/*
 * GAME SCENE
 * Copyright © 2021+ María López Ausín
 * @version 1.0.0
 */

#include "Game_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Scaling>
#include <basics/Rotation>
#include <basics/Translation>

using namespace basics;
using namespace std;

namespace flappyfish
{

    Game_Scene::Game_Scene()
    {
        canvas_width  = 720;
        canvas_height =  1280;
    }

    bool Game_Scene::initialize ()
    {
        game_state = PLAYING;
        state     = LOADING;
        suspended = false;
        hasStartedPlaying = false;

        punctuation = 0;
        pause_button.position = {(float)canvas_width * 0.1f, (float)canvas_height * 0.9f};

        //Moñeco
        x         = 100;
        y         = (float)canvas_height/2;

        //Fondo
        bgx         = 1280.0f/2;
        bgy         = 1280.0f/2;
        bg2x        = bgx + 1280;

        //Seed para el random
        srand (unsigned(time(nullptr)));

        //Posición de las tuberías
        pipes[0].pos = pipes[pipes_size / 2].pos =      {(canvas_width + 250), canvas_height/2 - DISTANCE_UP/2};

        //Todas las tuberías de arriba van después de las tuberías de abajo, por lo que
        // la tubería 0 va con la 3, la 1 con la 4, etc.
        pipes[pipes_size / 2].pos.coordinates.y() += DISTANCE_UP;

        //Las primeras tuberías van al medio, las siguientes en Y aleatorias
        for (unsigned i = 1; i < pipes_size / 2 ; ++i)
        {
            pipes[i].pos         = pipes[i + pipes_size / 2].pos =         {(pipes[i - 1].pos.coordinates.x() + DISTANCE_X), (random_Y_pos(pipes[i - 1].pos.coordinates.y()))};
            pipes[i + pipes_size / 2].pos.coordinates.y() += DISTANCE_UP;
        }

        return true;
    }

    void Game_Scene::suspend ()
    {
        suspended = true;
    }

    void Game_Scene::resume ()
    {
        suspended = false;
    }

    void Game_Scene::handle (Event & event)
    {
        if (state == RUNNING)
        {
            switch (event.id)
            {
                case ID(touch-started):
                {
                    //Para no empezar nada más se carga
                    if(!hasStartedPlaying) hasStartedPlaying = true;

                    //Animación pez cuando le das al tap para feedback e impulso

                    if(game_state == PLAYING){
                        flying ? flying = false : flying = true;
                        yForce = 5;
                    }


                    break;
                }
               case ID(touch-ended):
                {
                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                    if(game_state != PLAYING) //Opciones de Game Over y Pause
                    {

                        if (option_at (touch_location) == PLAY && game_state == GAME_OVER)
                        {
                           initialize();
                        }
                        else if (option_at (touch_location) == QUIT && game_state != PLAYING)
                        {
                            director.stop();
                        }
                        else if (option_at (touch_location) == PLAY && game_state == PAUSED)
                        {
                             game_state = PLAYING;
                        }
                    }

                    else //Botón de pause mientras juegas
                    {
                        if(     touch_location[0] > pause_button.position[0] - pause_button.size[0]  &&
                                touch_location[0] < pause_button.position[0] + pause_button.size[0]  &&
                                touch_location[1] > pause_button.position[1] - pause_button.size[1] &&
                                touch_location[1] < pause_button.position[1] + pause_button.size[1]  )
                        {
                           game_state = PAUSED;
                        }

                    }

                    break;
                }
            }
        }
    }

    void Game_Scene::update (float time)
    {
        switch (state)
        {
            case LOADING: load ();     break;
            case RUNNING: if(hasStartedPlaying) run  (time); break;
        }
    }

    void Game_Scene::render (basics::Graphics_Context::Accessor & context)
    {
        if (!suspended && state == RUNNING)
        {
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            if (canvas)
            {
                canvas->clear        ();

                if(background) //Dibuja los fondos uno tras otro
                {
                    canvas->fill_rectangle ({ bgx, bgy },   {background->get_width() , background->get_height() }, background.get ());
                    canvas->fill_rectangle ({ bg2x, bgy },  {background->get_width() , background->get_height() }, background.get ());
                }

                if(atlas)
                {
                    //Dibuja las tuberías
                    for (int i = 0; i < pipes_size; ++i)
                    {

                         if(i < pipes_size / 2) //Las primeras son las de abajo

                             draw_slice (canvas, pipes[i].pos, *atlas, ID(pipes.pipeup) );

                         else //Las de después sus parejas
                             draw_slice (canvas, pipes[i].pos, *atlas, ID(pipes.pipedown) );

                    }

                    if(flying)
                        draw_slice(canvas, {x,y}, *atlas, ID(player.1));

                    else
                        draw_slice(canvas, {x,y}, *atlas, ID(player.2));

                }

                if(font) //Muestra puntuación
                {
                    Text_Layout punctuation_text(*font, to_wstring(punctuation));
                    canvas->draw_text({canvas_width/2, canvas_height*0.95f}, punctuation_text, TOP | CENTER);
                }

                if (atlas_menu) //Botones y menús según el estado de juego
                {
                    if(game_state == GAME_OVER)
                    {
                        draw_slice(canvas, {canvas_width/2, canvas_height/2}, *atlas_menu, ID(game_over));

                        options[PLAY   ].slice = atlas_menu->get_slice (ID(replay_but)   );
                        options[QUIT ].slice = atlas_menu->get_slice (ID(quit_but) );

                        float heigth = 0;

                        for (unsigned i = 0; i < number_of_options; ++i)
                        {
                            options[i].position = {canvas_width/2, canvas_height/2 + heigth};
                            canvas->fill_rectangle ({ options[i].position }, { options[i].slice->width, options[i].slice->height }, options[i].slice, CENTER | TOP);
                            heigth -= options[i].slice->height;
                        }
                    }
                    else if (game_state == PAUSED)
                    {
                        draw_slice(canvas, {canvas_width/2, canvas_height/2}, *atlas_menu, ID(pause));

                        options[PLAY   ].slice = atlas_menu->get_slice (ID(continue_but)   );
                        options[QUIT ].slice = atlas_menu->get_slice (ID(quit_but) );

                        float heigth = 0;
                        for (unsigned i = 0; i < number_of_options; ++i)
                        {
                            options[i].position = {canvas_width/2, canvas_height/2 + heigth};
                            canvas->fill_rectangle ({ options[i].position }, { options[i].slice->width, options[i].slice->height }, options[i].slice, CENTER | TOP);
                            heigth -= options[i].slice->height;
                        }
                    }
                    else if(game_state == PLAYING)
                    {
                        draw_slice(canvas, {canvas_width*0.1f, canvas_height*0.9f}, *atlas_menu, ID(pause_but));
                    }
                }
            }
        }
    }

    void Game_Scene::load ()
    {
        if (!suspended)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                background = Texture_2D::create (ID(bg), context, "fondo.png");

                font.reset (new Raster_Font("myfont.fnt", context));
                atlas.reset (new Atlas("game-assets.sprites", context));
                atlas_menu.reset (new Atlas("menu-sprites.sprites", context));

                if (background && atlas->good() && font->good() && atlas_menu->good())
                {

                    //Además de cargar las imágenes, cuando están listas asigno ciertos valores
                    //según el tamaño de los slices
                    dimensions = {atlas->get_slice (ID(pipes.pipedown))->width,
                                  atlas->get_slice (ID(pipes.pipedown))->height};

                    pause_button.size = {atlas_menu->get_slice (ID(pause_but))->width,
                                         atlas_menu->get_slice (ID(pause_but))->height};

                    context->add(background);

                    state = RUNNING;
                }
            }
        }
    }

    void Game_Scene::run (float dT)
    {
        if(game_state == PLAYING)
        {
            //Movimiento en Y del pez con gravedad
            yForce  -= GRAVITY * dT;
            y       += yForce * 1.5f;

            //Se mueve el fondo poco a poco
            bgx     -= dT*BGSPEED;
            bg2x    -= dT*BGSPEED;

            //Cuando los sprites del bg se salen de la pantalla +5px, se recolocan
            if      (bgx + background->get_width()/2 + 5 < 0)               bgx = bg2x + background->get_width();
            else if (bg2x + background->get_width()/2 + 5 < 0)              bg2x = bgx + background->get_width();


            //Se mueven las tuberías más rápido
            for (int i = 0; i < pipes_size; ++i)
            {
                pipes[i].pos.coordinates.x() -= dT* PIPE_SPEED;
            }

            //Comprobación salir de la pantalla de las tuberías para recolocarlas
            unsigned index = 0;
            for (index = 0; index < pipes_size / 2; ++index)
            {
                if (pipes[index].pos.coordinates.x() + dimensions.coordinates.x()/2 <= 0)
                {
                    unsigned previous_pos = 0;

                    if (index == 0)
                        previous_pos = pipes_size - 1;
                    else
                        previous_pos = index - 1;

                    pipes[index].pos.coordinates.x() = pipes[index + pipes_size / 2].pos.coordinates.x() = pipes[previous_pos].pos.coordinates.x() + DISTANCE_X;
                    pipes[index].pos.coordinates.y() = random_Y_pos(pipes[previous_pos].pos.coordinates.y());
                    pipes[index + pipes_size / 2].pos.coordinates.y() = pipes[index].pos.coordinates.y() + DISTANCE_UP;

                    //Suma más uno cuando la tubría sale de la pantalla
                    add_punctuation();
                }

            }

            //Colisiones con el pez, he decidido dejar un poco de cancha y en vez de chocarte
            //con un solo píxel contra la tubería, tienes más cancha
          for (int i = 0; i < pipes_size; ++i)
          {
              if(x > pipes[i].pos[0] - dimensions[0]/2
              && x < pipes[i].pos[0] + dimensions[0]/2
              && y > pipes[i].pos[1] - dimensions[1]/2
              && y < pipes[i].pos[1] + dimensions[1]/2)
              {
                  game_state = GAME_OVER;
              }
          }

            //Comprobación Game Over si te sales de la pantalla
            if (y < 0 + 50 || y> canvas_height + 50) game_state = GAME_OVER;
        }

    }

    void Game_Scene::draw_slice (Canvas * canvas, const basics::Point2f & where, basics::Atlas & atlas, basics::Id slice_id)
    {
        const Atlas::Slice * slice = atlas.get_slice (slice_id);

        if (slice)
        {
            canvas->fill_rectangle (where, { slice->width, slice->height}, slice);
        }
    }

    void Game_Scene::add_punctuation()
    {
        ++punctuation;
    }


    int Game_Scene::option_at (const Point2f & point)
    {
        //Para los botones de Continue, Quit y Replay
        for (int index = 0; index < 2; ++index)
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
