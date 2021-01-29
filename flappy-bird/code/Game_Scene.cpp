/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Game_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Log>
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
        state     = LOADING;
        suspended = false;

        hasStartedPlaying = false;

        //Moñeco
        x         = 100;
        y         = canvas_height/2;

        //Fondo
        bgx = 1280/2;
        bgy = 1280/2;
        bg2x = bgx+1280;

        //Tuberías
        pipes1.pos = {canvas_width/2, canvas_height/2};



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
                case ID(touch-moved):
                case ID(touch-ended):{
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
            case PAUSED: gameOver(); break;
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
                    canvas->fill_rectangle ({ bgx, bgy }, {1280 , 1280 }, background.get ());
                    canvas->fill_rectangle ({ bg2x, bgy }, {1280 , 1280 }, background.get ());
                }
                if (texture) canvas->fill_rectangle ({ x, y }, { 100, 100 }, texture.get ());
                if (pipesTexture) canvas->fill_rectangle ({ pipes1.pos}, {100, 100 }, pipesTexture.get ());

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
                pipesTexture = Texture_2D::create (ID(pipes), context, "game-scene/Pipes.png");

                if (texture && background && pipesTexture)
                {
                    context->add (texture);
                    context->add(background);
                    context->add(pipesTexture);

                    state = RUNNING;
                }
            }
        }
    }

    void Game_Scene::run (float dT)
    {
        //Movimiento en Y del pez con gravedad
        yForce -= GRAVITY * dT;
        y += yForce * 1.5f;



        //Se mueve el fondo poco a poco
        bgx -= dT*BGSPEED;
        bg2x -= dT*BGSPEED;

        //Cuando los sprites del bg se salen de la pantalla +5px, se recolocan
        if(bgx + (1280/2) + 5 < 0) bgx = bg2x + 1280;
        else if(bg2x + (1280/2) + 5 < 0) bg2x = bgx + 1280;


        //Se mueven las tuberías más rápido
        pipes1.pos.coordinates.x() -=dT*PIPESPEED;




        //Comprobación Game Over
        if( y < 50) state = PAUSED;

    }

    void Game_Scene::gameOver()
    {


    }



}
