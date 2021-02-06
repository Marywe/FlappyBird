/*
 * MENU SCENE
 * Copyright © 2021+ María López Ausín
 * @version 1.0.0
 */

#include "Menu_Scene.hpp"
#include "Game_Scene.hpp"

#include <basics/Canvas>
#include <basics/Director>
#include <basics/Transformation>

using namespace basics;
using namespace std;

namespace flappyfish
{

    Menu_Scene::Menu_Scene()
    {
        state         = LOADING;
        suspended     = true;
        canvas_width  = 720;
        canvas_height =  1280;
    }

    // ---------------------------------------------------------------------------------------------

    bool Menu_Scene::initialize ()
    {
        for (auto & option : options)
        {
            option.is_pressed = false;
        }

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::handle (basics::Event & event)
    {
        if (state == READY)                     // Se descartan los eventos cuando la escena está LOADING
        {
            switch (event.id)
            {
                case ID(touch-started):
                case ID(touch-moved):
                {
                    // Se determina qué opción se ha tocado:

                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };
                    int     option_touched = option_at (touch_location);

                    // Solo se puede tocar una opción a la vez (para evitar selecciones múltiples),
                    // por lo que solo una se considera presionada (el resto se "sueltan"):

                    for (int index = 0; index < number_of_options; ++index)
                    {
                        options[index].is_pressed = index == option_touched;
                    }

                    break;
                }

                case ID(touch-ended):           // El usuario deja de tocar la pantalla
                {
                    // Se "sueltan" todas las opciones:
                    for (auto & option : options) option.is_pressed = false;

                    // Se determina qué opción se ha dejado de tocar la última y se actúa como corresponda:
                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                    if (option_at (touch_location) == PLAY)
                    {
                        director.run_scene (shared_ptr< Scene >(new Game_Scene));
                    }
                    else if (option_at (touch_location) == QUIT)
                    {
                        director.stop();
                    }

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::update (float time)
    {
        if (!suspended) if (state == LOADING)
            {
                Graphics_Context::Accessor context = director.lock_graphics_context ();

                if (context)
                {
                    background = Texture_2D::create (ID(bg), context, "game-scene/fondo.png");
                    atlas.reset (new Atlas("menu-sprites.sprites", context));

                    // Si el atlas se ha podido cargar el estado es READY y, en otro caso, es ERROR:

                    if(atlas->good () && background)
                    {
                        context->add(background);
                        state = READY;
                    }

                    // Si el atlas está disponible, se inicializan los datos de las opciones del menú:

                    if (state == READY)
                    {
                        configure_options ();
                    }
                }
            }
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            if (canvas)
            {
                canvas->clear ();
                canvas->set_color(0,0,1);

                if (state == READY)
                {

                    canvas->fill_rectangle ({ canvas_width/2, canvas_height/2 },   {background->get_width() , background->get_height() }, background.get ());

                    const Atlas::Slice * slice = atlas->get_slice (ID(title));
                    canvas->fill_rectangle ({canvas_width/2, canvas_height*0.7f }, { slice->width, slice->height }, slice);

                    // Se dibuja el slice de cada una de las opciones del menú:
                    for (auto & option : options)
                    {
                        canvas->set_transform
                                (
                                        scale_then_translate_2d
                                                (
                                                        option.is_pressed ? 0.75f : 1.f,              // Escala de la opción
                                                        { option.position[0], option.position[1] }      // Traslación
                                                )
                                );

                        canvas->fill_rectangle ({ 0.f, 0.f }, { option.slice->width, option.slice->height }, option.slice, CENTER | TOP);
                    }




                    // Se restablece la transformación aplicada a las opciones para que no afecte a
                    // dibujos posteriores realizados con el mismo canvas:

                    canvas->set_transform (Transformation2f());
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::configure_options ()
    {
        // Se asigna un slice del atlas a cada opción del menú según su ID:

        options[PLAY   ].slice = atlas->get_slice (ID(PLAY)   );
        options[QUIT ].slice = atlas->get_slice (ID(QUIT) );

        // Se calcula la altura total del menú:
        float menu_height = 0;

        for (auto & option : options) menu_height += option.slice->height;

        // Se calcula la posición del borde superior del menú en su conjunto de modo que
        // quede centrado verticalmente:
        float option_top = canvas_height / 2.f + menu_height / 2.f;

        // Se establece la posición del borde superior de cada opción:
        for (unsigned index = 0; index < number_of_options; ++index)
        {
            options[index].position = Point2f { canvas_width / 2.f, option_top };

            option_top -= options[index].slice->height;
        }

        // Se restablece la presión de cada opción:
        initialize ();
    }

    // ---------------------------------------------------------------------------------------------

    int Menu_Scene::option_at (const Point2f & point)
    {
        for (int index = 0; index < number_of_options; ++index)
        {
            const Option & option = options[index];

            if
                    (
                    point[0] > option.position[0] - option.slice->width  &&
                    point[0] < option.position[0] + option.slice->width  &&
                    point[1] > option.position[1] - option.slice->height &&
                    point[1] < option.position[1] + option.slice->height
                    )
            {
                return index;
            }
        }

        return -1;
    }

}
