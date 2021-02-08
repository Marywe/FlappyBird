/*
 * GAME SCENE
 * Copyright © 2021+ María López Ausín
 * @version 1.0.0
 */

#ifndef GAME_SCENE_HEADER
#define GAME_SCENE_HEADER

#include <memory>
#include <basics/Scene>
#include <basics/Texture_2D>
#include <basics/Atlas>
#include <basics/Canvas>

namespace flappyfish
{
    using basics::Point2f;
    using basics::Size2f;

    using basics::Atlas;

    class Game_Scene : public basics::Scene
    {
        typedef std::unique_ptr< basics::Raster_Font> Font_Handle;
        typedef std::unique_ptr< basics::Atlas       > Atlas_Handle;
        typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;

    private:

        enum State
        {
            LOADING,
            RUNNING,
        };

        enum Game_state {
            PLAYING,
            GAME_OVER,
            PAUSED
        };

        enum Option_Id
        {
            REPLAY,
            QUIT,
        };

        State          state;
        Game_state     game_state;
        bool           suspended;

        unsigned       canvas_width;
        unsigned       canvas_height;

        bool hasStartedPlaying;

        bool flying = false;
        Texture_Handle texture;
        Texture_Handle background;
        Atlas_Handle atlas, atlas_menu;


        Font_Handle font;
        unsigned punctuation;


        float          bgx, bgy, bg2x;
        float          x, y;
        float          yForce;

        const int BGSPEED = 100;
        static constexpr float GRAVITY = 9.8f;

        struct Pipes {
            Point2f pos;
        };

        Point2f dimensions = {0,0};
        const int PIPE_SPEED = 130;
        static constexpr float DISTANCE_UP = 1100;
        static constexpr float DISTANCE_X = 400;

        static const unsigned pipes_size = 6;
        Pipes pipes [pipes_size];


        struct Option
        {
            const Atlas::Slice * slice;
            Point2f position;
        };

        static const unsigned number_of_options = 2;
        Option   options[number_of_options];                ///< Datos de las opciones del menú

        struct Pause_Button {
            Point2f position;
            Size2f size;
        };
        Pause_Button pause_button;

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
        void draw_slice (basics::Canvas * canvas, const basics::Point2f & where, basics::Atlas & atlas, basics::Id slice_id);
        void add_punctuation();
        int option_at (const Point2f & point);


        inline float random_Y_pos (const float previous_Y) ///< Saca una random Y para las tuberías que se van colocando al final según la posición en Y de la anterior
        {
            return std::max(-200.0f, std::min( previous_Y + rand() % 500 + (-350) , 400.0f)); ///< Clamp entre dos valores para que no se salga en Y de la pantalla
        }
    };

}
#endif
