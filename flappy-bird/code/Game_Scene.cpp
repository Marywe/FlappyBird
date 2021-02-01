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
        state     = LOADING;
        suspended = false;

        hasStartedPlaying = false;

        //Moñeco
        x         = 100;
        y         = canvas_height/2;

        //Fondo
        bgx         = 1280/2;
        bgy         = 1280/2;
        bg2x        = bgx + 1280;

        //Tuberías
        PD1.pos = PU1.pos =      { (canvas_width /2), canvas_height};
        PD2.pos = PU2.pos =      {(PD1.pos.coordinates.x() + DISTANCE_X), (random_Y_pos(PD1.pos.coordinates.y()))};
        PD3.pos = PU3.pos =      {(PD2.pos.coordinates.x() + DISTANCE_X), (random_Y_pos(PD2.pos.coordinates.y()))};

        PU1.pos.coordinates.y() += DISTANCE_UP;
        PU2.pos.coordinates.y() += DISTANCE_UP;
        PU3.pos.coordinates.y() += DISTANCE_UP;

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
            case GAME_OVER: game_over(); break;
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

                draw_slice (canvas, PD1.pos, *atlas, ID(pipes.pipedown) );
                draw_slice (canvas, PU1.pos, *atlas, ID(pipes.pipeup) );
                draw_slice (canvas, PD2.pos, *atlas, ID(pipes.pipedown) );
                draw_slice (canvas, PU2.pos, *atlas, ID(pipes.pipeup) );
                draw_slice (canvas, PD3.pos, *atlas, ID(pipes.pipedown) );
                draw_slice (canvas, PU3.pos, *atlas, ID(pipes.pipeup) );


                if(font)
                {
                    Text_Layout sample_text(*font, L"SAMPLE");
                    canvas->draw_text({0.f, 0.f}, sample_text, BOTTOM | LEFT);
                    canvas->draw_text({0.f, canvas_height}, sample_text, TOP | LEFT);
                    canvas->draw_text({canvas_width, 0.f}, sample_text, BOTTOM | RIGHT);
                    canvas->draw_text({canvas_width, canvas_height}, sample_text, TOP | RIGHT);
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
        PD1.pos.coordinates.x() -= dT * PIPE_SPEED;
        PD2.pos.coordinates.x() -= dT * PIPE_SPEED;
        PD3.pos.coordinates.x() -= dT * PIPE_SPEED;
        PU1.pos.coordinates.x() -= dT * PIPE_SPEED;
        PU2.pos.coordinates.x() -= dT * PIPE_SPEED;
        PU3.pos.coordinates.x() -= dT * PIPE_SPEED;

        //Al salirse de la pantalla, se pone tras la última             ///<Mas width
        if      (PD1.pos.coordinates.x () +5 < 0 )
        {
            PD1.pos.coordinates = {PD3.pos.coordinates.x() + DISTANCE_X, random_Y_pos(PD3.pos.coordinates.y())};
            PU1.pos.coordinates = {PD1.pos.coordinates.x(), PD1.pos.coordinates.y() + DISTANCE_UP};
        }
        else if (PD2.pos.coordinates.x () +5 < 0 )
        {
            PD2.pos.coordinates = {PD1.pos.coordinates.x() + DISTANCE_X, random_Y_pos(PD1.pos.coordinates.y())};
            PU2.pos.coordinates = {PD2.pos.coordinates.x(), PD2.pos.coordinates.y() + DISTANCE_UP};

        }
        else if (PD3.pos.coordinates.x () +5 < 0 )
        {
            PD3.pos.coordinates = {PD2.pos.coordinates.x() + DISTANCE_X, random_Y_pos(PD2.pos.coordinates.y())};
            PU3.pos.coordinates = {PD3.pos.coordinates.x(), PD3.pos.coordinates.y() + DISTANCE_UP};
        }


       // if(x + 200 <= PD1.pos.coordinates.x()) state = GAME_OVER;



        //Comprobación Game Over
        if (y < 0) state = GAME_OVER;

    }

    void Game_Scene::game_over()
    {


    }

    void Game_Scene::draw_slice (Canvas * canvas, const basics::Point2f & where, basics::Atlas & atlas, basics::Id slice_id)
    {
        const Atlas::Slice * slice = atlas.get_slice (slice_id);

        if (slice)
        {
            canvas->fill_rectangle (where, { slice->width, slice->height}, slice);
        }
    }

    void Game_Scene::add_punctuation(){

    }
}
