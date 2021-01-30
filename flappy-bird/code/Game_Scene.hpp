/*
 * GAME SCENE
 * Copyright © 2020+ María López Ausín
 * @version 1.0.0
 */

#include <memory>
#include <basics/Scene>
#include <basics/Texture_2D>
#include <basics/Atlas>
#include <basics/Canvas>



namespace flappyfish
{
    using basics::Point2f;
    using basics::Atlas;

    class Game_Scene : public basics::Scene
    {
        typedef std::unique_ptr< basics::Atlas       > Atlas_Handle;
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
        Atlas_Handle atlas;

        float          bgx, bgy, bg2x;
        float          x, y;
        float          yForce;

        const int BGSPEED = 10;
        static constexpr float GRAVITY = 9.8f;

        struct Pipes {
            Point2f pos;
            bool up = true;
            const Atlas::Slice * slice;
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
        void draw_slice (basics::Canvas * canvas, const basics::Point2f & where, basics::Atlas & atlas, basics::Id slice_id);

    };

}
