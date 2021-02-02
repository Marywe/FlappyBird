/*
 * GAME SCENE
 * Copyright © 2021+ María López Ausín
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
        typedef std::unique_ptr< basics::Raster_Font> Font_Handle;
        typedef std::unique_ptr< basics::Atlas       > Atlas_Handle;
        typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;

    public:

        enum State
        {
            LOADING,
            RUNNING,
            GAME_OVER
        };

        State          state;
        bool           suspended;

        unsigned       canvas_width;
        unsigned       canvas_height;

        bool hasStartedPlaying;

        Texture_Handle texture;
        Texture_Handle background;
        Font_Handle font;
        Atlas_Handle atlas;


        float          bgx, bgy, bg2x;
        float          x, y;
        float          yForce;

        const int BGSPEED = 10;
        static constexpr float GRAVITY = 9.8f;

        struct Pipes {
            Point2f pos;
            const Atlas::Slice * slice;
        };
        const int PIPE_SPEED = 100;
        static constexpr float DISTANCE_UP = 1100;
        static constexpr float DISTANCE_X = 400;

        unsigned array_size = 6;
        Pipes pipes [6];

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
        void game_over ();
        void draw_slice (basics::Canvas * canvas, const basics::Point2f & where, basics::Atlas & atlas, basics::Id slice_id);
        void add_punctuation();

        inline float random_Y_pos (const float previous_Y) ///< Saca una random Y para las tuberías que se van colocando al final según la posición en Y de la anterior
        {
            return std::max(-600.0f, std::min( previous_Y + rand() % 600 + (-300) , 200.0f)); ///< Clamp entre dos valores para que no se salga en Y de la pantalla
        }
    };

}
