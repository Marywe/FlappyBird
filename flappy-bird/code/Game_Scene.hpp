/*
 * GAME SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include <memory>
#include <basics/Scene>
#include <basics/Texture_2D>


namespace flappyfish
{
    using basics::Point2f;

    class Game_Scene : public basics::Scene
    {

        typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;

    public:


        enum State
        {
            LOADING,
            RUNNING,
            PAUSED
        };

        State          state;
        bool           suspended;

        unsigned       canvas_width;
        unsigned       canvas_height;

        bool hasStartedPlaying;

        Texture_Handle texture;
        Texture_Handle background;
        Texture_Handle pipesTexture;


        float          bgx, bgy, bg2x;
        float          x, y;
        float          yForce;

        const int BGSPEED = 10;
        static constexpr float GRAVITY = 9.8f;

        struct Pipes {
            Point2f pos;
            bool up = true;
        };
        const int PIPESPEED = 50;
        Pipes pipes1, pipes2, pipes3;

    public:

        Game_Scene();

        basics::Size2u get_view_size () override
        {
            return { canvas_width, canvas_height };
        }

        bool initialize () override;
        void suspend    () override;
        void resume     () override;

        void handle     (basics::Event & event) override;
        void update     (float time) override;
        void render     (basics::Graphics_Context::Accessor & context) override;

    private:

        void load ();
        void run  (float time);
        void gameOver ();

    };

}
