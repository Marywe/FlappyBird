/*
 * MENU SCENE
 * Copyright © 2021+ María López Ausín
 * @version 1.0.0
 */

#ifndef MENU_SCENE_HEADER
#define MENU_SCENE_HEADER

#include <memory>
#include <basics/Atlas>
#include <basics/Canvas>
#include <basics/Point>
#include <basics/Scene>

namespace flappyfish
{
    using basics::Atlas;
    using basics::Canvas;
    using basics::Point2f;
    using basics::Size2f;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;


    class Menu_Scene : public basics::Scene
    {

        enum State
        {
            LOADING,
            READY,
            ERROR
        };

        enum Option_Id
        {
            PLAY,
            QUIT,
        };

        Texture_Handle background;

        struct Option
        {
            const Atlas::Slice * slice;
            Point2f position;
            float   is_pressed;
        };

        static const unsigned number_of_options = 2;

    private:

        State    state;
        bool     suspended;

        unsigned canvas_width;
        unsigned canvas_height;

        Option   options[number_of_options];         //Opciones del menú
        std::unique_ptr< Atlas > atlas;


        struct Help_Button {
            Point2f position;
            Size2f size;
        };
        Help_Button help_button;

        bool is_showing_help = false;
    public:

        Menu_Scene();

        basics::Size2u get_view_size () override
        {
            return { canvas_width, canvas_height };
        }

        bool initialize () override;

        void suspend () override
        {
            suspended = true;
        }

        void resume () override
        {
            suspended = false;
        }

        void handle (basics::Event & event) override;
        void update (float time) override;
        void render (Graphics_Context::Accessor & context) override;

        void configure_options ();              //Para los botones
        int option_at (const Point2f & point);  //Te dice qué opción pulsas

        void pressed_false(); //Para la configuración continua de los botones
    };

}

#endif
