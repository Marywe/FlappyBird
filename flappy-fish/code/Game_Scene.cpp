/*
 * GAME SCENE
 * Copyright © 2021+ María López Ausín
 * @version 1.0.0
 */

#include "Game_Scene.hpp"
#include "Menu_Scene.hpp"
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

        //Moñeco
        x         = 100;
        y         = canvas_height/2;

        //Fondo
        bgx         = 1280/2;
        bgy         = 1280/2;
        bg2x        = bgx + 1280;


        srand (unsigned(time(nullptr)));



        //Tuberías
        pipes[0].pos = pipes[array_size/2].pos =      { (canvas_width /2), canvas_height + 600};
        pipes[array_size/2].pos.coordinates.y() += DISTANCE_UP;

        for (unsigned i = 1; i < array_size / 2 ; ++i)
        {
            pipes[i].pos         = pipes[i + array_size/2].pos =         {(pipes[i-1].pos.coordinates.x() + DISTANCE_X), (random_Y_pos(pipes[i-1].pos.coordinates.y()))};
            pipes[i+ array_size/2].pos.coordinates.y() += DISTANCE_UP;
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
                    if(!hasStartedPlaying) hasStartedPlaying = true;
                    yForce = 5;
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
                if (texture) canvas->fill_rectangle ({ x, y }, { 100, 100 }, texture.get ());

                if(atlas)
                {
                    for (int i = 0; i < array_size; ++i)
                    {

                         if(i < array_size/2)

                             draw_slice (canvas, pipes[i].pos, *atlas, ID(pipes.pipedown) );
                         else
                             draw_slice (canvas, pipes[i].pos, *atlas, ID(pipes.pipeup) );

                    }
                }

                if(font)
                {
                    Text_Layout punctuation_text(*font, to_wstring(punctuation));
                    canvas->draw_text({canvas_width/2, canvas_height*0.95f}, punctuation_text, TOP | CENTER);
                }

                if(game_state == GAME_OVER) ; //render menú gameover
                else if (game_state == PAUSED) ; //render pause
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
                texture = Texture_2D::create (ID(test), context, "game-scene/test.png");
                background = Texture_2D::create (ID(bg), context, "game-scene/fondo.png");

                font.reset (new Raster_Font("menu-scene/myfont.fnt", context));
                atlas.reset (new Atlas("pipes.sprites", context));

                if (texture && background && atlas->good() && font->good())
                {
                    context->add (texture);
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
            for (int i = 0; i < array_size; ++i)
            {
                pipes[i].pos.coordinates.x() -= dT* PIPE_SPEED;
            }

            //Comprobación salir de la pantalla
            unsigned index = 0;
            for (index = 0; index < array_size / 2; ++index)
            {
                if (pipes[index].pos.coordinates.x() + dimensions[0] / 2 <= 0) //
                {
                    unsigned previous_pos = 0;

                    if (index == 0)
                        previous_pos = array_size - 1;
                    else
                        previous_pos = index - 1;

                    pipes[index].pos.coordinates.x() = pipes[index + array_size/2].pos.coordinates.x() = pipes[previous_pos].pos.coordinates.x() + DISTANCE_X;
                    pipes[index].pos.coordinates.y() = random_Y_pos(pipes[previous_pos].pos.coordinates.y());
                    pipes[index + array_size / 2].pos.coordinates.y() = pipes[index].pos.coordinates.y() + DISTANCE_UP;

                    add_punctuation();
                }

            }


            for (int i = 0; i < array_size; ++i)
            {
                if(x > pipes[index].pos[0] - dimensions[0]
                && x < pipes[index].pos[0] + dimensions[0]
                && y > pipes[index].pos[1] - dimensions[1]
                && y < pipes[index].pos[1] + dimensions[1])
                {
                    game_state = GAME_OVER;
                }
            }


            //Comprobación Game Over
            if (y < 0) game_state = GAME_OVER;
        }


    }

    void Game_Scene::game_over()
    {

    }

    void Game_Scene::draw_slice (Canvas * canvas, const basics::Point2f & where, basics::Atlas & atlas, basics::Id slice_id)
    {
        const Atlas::Slice * slice = atlas.get_slice (slice_id);

        if (dimensions.coordinates.x() == 0){
            dimensions = {slice->width, slice->height};
        }
        if (slice)
        {
            canvas->fill_rectangle (where, { slice->width, slice->height}, slice);
        }
    }

    void Game_Scene::add_punctuation()
    {
        ++punctuation;
    }
}
