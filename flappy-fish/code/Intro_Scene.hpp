/*
 * INTRO SCENE
 * Copyright © 2021+ María López Ausín
 * @version 1.0.0
 */

#ifndef INTRO_SCENE_HEADER
#define INTRO_SCENE_HEADER

#include <memory>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Texture_2D>
#include <basics/Timer>

namespace flappyfish
{

    using basics::Timer;
    using basics::Canvas;
    using basics::Texture_2D;
    using basics::Graphics_Context;

    class Intro_Scene : public basics::Scene
    {
        enum State
        {
            UNINITIALIZED,
            LOADING,
            FADING_IN,
            WAITING,
            FADING_OUT,
            FINISHED,
            ERROR
        };

    private:

        State    state;                                     ///< Estado de la escena.
        bool     suspended;                                 ///< true cuando la escena está en segundo plano y viceversa.

        unsigned canvas_width;                              ///< Ancho de la resolución virtual usada para dibujar.
        unsigned canvas_height;                             ///< Alto  de la resolución virtual usada para dibujar.

        Timer    timer;                                     ///< Cronómetro usado para medir intervalos de tiempo.

        float    opacity;                                   ///< Opacidad de la textura.

        std::shared_ptr < Texture_2D > logo_texture;        ///< Textura que contiene la imagen del logo.

    public:

        Intro_Scene()
        {
            state         = UNINITIALIZED;
            suspended     = true;
            canvas_width  = 720;
            canvas_height =  1280;
        }

        /**
         * Este método lo llama Director para conocer la resolución virtual con la que está
         * trabajando la escena.
         * @return Tamaño en coordenadas virtuales que está usando la escena.
         */
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

        void update (float time) override;

        void render (Graphics_Context::Accessor & context) override;

    private:

        void update_loading    ();
        void update_fading_in  ();
        void update_waiting    ();
        void update_fading_out ();

    };

}

#endif
