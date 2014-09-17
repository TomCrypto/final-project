#include "gui/window.h"

#include <stdexcept>

namespace gui
{
    /* ==================================================================== */

    const int target_fps = 60;
    static window* prog;
    static char** _argv;
    static int _argc;

    static void on_mouse_dispatch(int button, int state, int x, int y)
    {
        switch (state)
        {
            case GLUT_UP:
                return prog->on_mouse_up(button, x, y);
            case GLUT_DOWN:
                return prog->on_mouse_down(button, x, y);
        }
    }

    static void __button_cb(int glutButton, int glutState, int mouseX, int mouseY)
    {
        int retval = TwEventMouseButtonGLUT(glutButton, glutState, mouseX, mouseY);
        if (!retval) on_mouse_dispatch(glutButton, glutState, mouseX, mouseY);
    }

    static void __motion_cb(int mouseX, int mouseY)
    {
        int retval = TwEventMouseMotionGLUT(mouseX, mouseY);
        if (!retval) prog->on_mouse_move(mouseX, mouseY);
    }

    static void __keyboard_cb(unsigned char glutKey, int mouseX, int mouseY)
    {
        int retval = TwEventKeyboardGLUT(glutKey, mouseX, mouseY);
        if (!retval) prog->on_key_press(glutKey, mouseX, mouseY);
    }

    static void __special_cb(int glutKey, int mouseX, int mouseY)
    {
        int retval = TwEventSpecialGLUT(glutKey, mouseX, mouseY);
        if (!retval) prog->on_special(glutKey, mouseX, mouseY);
    }

    static void __resize_cb(int width, int height)
    {
        TwWindowSize(width, height);
        prog->on_resize(width, height);
    }

    static void __display_cb()
    {
        prog->on_display();
    }

    static void __update_cb(int value)
    {
        glutTimerFunc((int)(1000.0f / target_fps), __update_cb, 0);
        prog->on_update();
    }

    /* ==================================================================== */

    void window::initialize(int argc, char* argv[])
    {
        fftwf_init_threads();
        fftwf_plan_with_nthreads(4);
        FreeImage_Initialise();

        _argv = argv;
        _argc = argc;
    }

    void window::finalize()
    {
        FreeImage_DeInitialise();
        fftwf_cleanup_threads();
        fftwf_cleanup();
    }

    /* ==================================================================== */

    window::window(const std::string& window_title,
                   const std::pair<int, int>& dims)
    {
        if (prog != nullptr)
            throw std::logic_error("program already running");

        glutInit(&_argc, _argv);
        glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	    glutInitWindowSize(dims.first, dims.second);
        glutInitWindowPosition(
            (glutGet(GLUT_SCREEN_WIDTH)-dims.first)/2,
            (glutGet(GLUT_SCREEN_HEIGHT)-dims.second)/2);
	    m_window = glutCreateWindow(window_title.c_str());

        GLenum err = glewInit();
        if (err != GLEW_OK)
	        throw std::runtime_error("Failed to initialize GLEW: "
                                   + std::string((char *)glewGetErrorString(err))
                                   + ".");

        if (!GLEW_VERSION_2_1)
            throw std::runtime_error("Sorry, OpenGL 2.1 needed!\n");

        if (TwInit(TW_OPENGL, NULL) != 1)
            throw std::runtime_error("Failed to initialize AntTweakBar.");

        TwGLUTModifiersFunc(glutGetModifiers);
        glutPassiveMotionFunc(__motion_cb);
        glutKeyboardFunc(__keyboard_cb);
        glutSpecialFunc(__special_cb);
        glutDisplayFunc(__display_cb);
        glutReshapeFunc(__resize_cb);
        glutMotionFunc(__motion_cb);
        glutMouseFunc(__button_cb);

        on_init();
        prog = this;

        glutTimerFunc(1, __update_cb, 0);
    }

    window::~window()
    {
        on_free();

        TwTerminate();
        glutDestroyWindow(m_window);
    }

    void window::run()
    {
        glutMainLoop();
    }

    int window::width()
    {
        return glutGet(GLUT_WINDOW_WIDTH);
    }

    int window::height()
    {
        return glutGet(GLUT_WINDOW_HEIGHT);
    }

    /* ==================================================================== */

    void window::on_init()
    {
        m_bar = new main_bar("main");
        m_bar->set_title("Configuration");

        m_obj = new Model("Teapot.obj");

        glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluPerspective(G308_FOVY, (double) width() / (double) height(), G308_ZNEAR_3D, G308_ZFAR_3D);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();

	    gluLookAt(0.0, 0.0, 7.0, 0.0, 0., 0.0, 0.0, 1.0, 0.0);

        float direction[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	    float diffintensity[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	    float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	    glLightfv(GL_LIGHT0, GL_POSITION, direction);
	    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffintensity);
	    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	    glEnable(GL_LIGHT0);

        buf = new fbuffer(width(), height());

        m_cam = camera(width(), height(), 70 * M_PI / 180, glm::vec3(0, 0, -1), glm::vec3(0, 0, 1));
    }

    void window::on_free()
    {
        delete buf;
        delete m_bar;
		delete m_obj;
    }

    static bool pressed = false;

    void window::on_mouse_up(int button, int x, int y)
    {
        if (button == GLUT_LEFT_BUTTON)
            pressed = false;
    }

    void window::on_mouse_down(int button, int x, int y)
    {
        if (button == GLUT_LEFT_BUTTON)
            pressed = true;
    }

    void window::on_mouse_move(int x, int y)
    {
        static int last_x = 0;
        static int last_y = 0;

        if (last_x == 0)
            last_x = x;

        if (last_y == 0)
            last_y = y;

        if (pressed)
        {
            m_cam.turn_horizontal(((float)x - last_x) * 0.002f);
            m_cam.turn_vertical(((float)y - last_y) * 0.002f);
        }

        last_x = x;
        last_y = y;
    }

    void window::on_key_press(unsigned char key, int x, int y)
    {
        if(key=='r') //when the r key is pressed
        {
            m_bar->rotation += 5;
            m_bar->refresh();
        }

        if (key == 'w')
        {
            m_cam.move_forward(0.1);
        }

        if (key == 's')
        {
            m_cam.move_forward(-0.1);
        }

        if (key == 'a')
        {
            m_cam.move_left(0.1);
        }

        if (key == 'd')
        {
            m_cam.move_right(0.1);
        }
    }

    void window::on_special(int key, int x, int y)
    {
        // TODO: do something here?
        // actually I have no idea if this will be useful
    }

    void window::on_resize(int w, int h)
    {
        TwWindowSize(w, h);
        buf->resize(w, h);

        glViewport(0, 0, w, h);
        m_cam.resize(w, h);
    }

    void window::on_display()
    {
        // later on this might look like this:
        // - bind framebuffer
        // - clear framebuffer
        // - render scene (model + sky)
        // - render lens flares into framebuffer
        // - tonemap framebuffer into backbuffer
        // - draw the bar
        // - glutSwapBuffers()

        buf->bind();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	    glEnable(GL_DEPTH_TEST);
	    glEnable(GL_LIGHTING);
	    glEnable(GL_COLOR_MATERIAL);
	    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	    glShadeModel(GL_SMOOTH);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(m_cam.proj()));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(m_cam.view()));

	    GLenum err = glGetError();
	    if (err != GL_NO_ERROR) {
		    printf("%s\n", gluErrorString(err));
	    }

		// tweakbar example - read rotation from the GUI bar
		//skeleton->angle = m_bar->rotation;

		//skeleton->display();
		glColor3f(0.0f, 1.0f, 0.0f);
		m_obj->display();

        glDisable(GL_DEPTH_TEST);
	    glDisable(GL_LIGHTING);
	    glDisable(GL_COLOR_MATERIAL);

        buf->render(m_bar->exposure);

        TwDraw();

        glutSwapBuffers();
    }

    void window::on_update()
    {
        glutPostRedisplay();
    }
}
