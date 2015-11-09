
#include <nanogui/nanogui.h>
#if defined(WIN32)
#include <windows.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif
#include <nanogui/glutil.h>
#include <iostream>
#include <vector>
#include <memory>
#include "matrix.h"
#include "shader.h"
#include "crosshair.h"
#include "block.h"
#include "chunk.h"
#include "chunk_shader.h"
#include "client.h"

#define KONSTRUCTS_APP_TITLE "Konstructs"
#define KONSTRUCTS_APP_WIDTH 1024
#define KONSTRUCTS_APP_HEIGHT 768
#define MAX_PENDING_CHUNKS 128

using std::cout;
using std::cerr;
using std::endl;
using namespace konstructs;

class Konstructs: public nanogui::Screen {
public:
    Konstructs() :
        nanogui::Screen(Eigen::Vector2i(KONSTRUCTS_APP_WIDTH, KONSTRUCTS_APP_HEIGHT), KONSTRUCTS_APP_TITLE),
        crosshair(mSize.y(), mSize.x()),
        player(0, Vector3f(0.0f, 0.0f, 0.0f), 0.0f, 0.0f),
        px(0), py(0),
        model_factory(blocks),
        client("tetestte", "123456789", "localhost") {
        client.chunk(MAX_PENDING_CHUNKS);
        using namespace nanogui;
        performLayout(mNVGContext);
        glfwSetInputMode(mGLFWWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    ~Konstructs() {
    }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            setVisible(false);
            return true;
        } else if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
            init_menu();
            performLayout(mNVGContext);
        }
        return false;
    }

    virtual void draw(NVGcontext *ctx) {
        Screen::draw(ctx);
    }

    virtual void drawContents() {
        using namespace nanogui;
        handle_network();
        handle_keys();
        handle_mouse();
        for(auto model : model_factory.fetch_models()) {
            chunk.add(model);
        }
        chunk.render(player, mSize.y(), mSize.x());
        crosshair.render();
    }

private:

    void handle_mouse() {
        int exclusive =
             glfwGetInputMode(mGLFWWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
        if (exclusive && (px || py)) {
            double mx, my;
            glfwGetCursorPos(mGLFWWindow, &mx, &my);
            float m = 0.0025;
            float drx = (mx - px) * m;
            float dry = (my - py) * m;

            player.rotate_x(dry);
            player.rotate_y(drx);
            px = mx;
            py = my;
        }
        else {
            glfwGetCursorPos(mGLFWWindow, &px, &py);
        }
    }

    void handle_keys() {
        int sx = 0;
        int sz = 0;

        if(glfwGetKey(mGLFWWindow, GLFW_KEY_W)) {
            sz--;
        }
        if(glfwGetKey(mGLFWWindow, GLFW_KEY_S)) {
            sz++;
        }
        if(glfwGetKey(mGLFWWindow, GLFW_KEY_A)) {
            sx--;
        }
        if(glfwGetKey(mGLFWWindow, GLFW_KEY_D)) {
            sx++;
        }
        client.position(player.update_position(sz, sx), player.rx(), player.ry());

    }

    void handle_network() {
        for(auto packet : client.receive(10)) {
            handle_packet(packet);
        }
    }

    void handle_packet(shared_ptr<konstructs::Packet> packet) {
        switch(packet->type) {
        case 'U':
            handle_player_packet(packet->to_string());
            break;
        case 'W':
            handle_block_type(packet->to_string());
            break;
        case 'C':
            handle_chunk(packet);
            break;
        default:
            cout << "UNKNOWN: " << packet->type << endl;
            break;
        }
    }

    void handle_player_packet(const string &str) {
        int pid;
        float x, y, z, rx, ry;

        if(sscanf(str.c_str(), ",%d,%f,%f,%f,%f,%f",
                  &pid, &x, &y, &z, &rx, &ry) != 6)
            throw std::runtime_error(str);
        player = Player(pid, Vector3f(x, 250.0, z), rx, ry);
    }

    void handle_block_type(const string &str) {
        int w, obstacle, transparent, left, right, top, bottom, front, back;
        char shape[16];
        if(sscanf(str.c_str(), ",%d,%15[^,],%d,%d,%d,%d,%d,%d,%d,%d",
                  &w, shape, &obstacle, &transparent, &left, &right,
                  &top, &bottom, &front, &back) != 10)
            throw std::runtime_error(str);
        blocks.is_plant[w] = strncmp(shape, "plant", 16) == 0;
        blocks.is_obstacle[w] = obstacle;
        blocks.is_transparent[w] = transparent;
        blocks.blocks[w][0] = left;
        blocks.blocks[w][1] = right;
        blocks.blocks[w][2] = top;
        blocks.blocks[w][3] = bottom;
        blocks.blocks[w][4] = front;
        blocks.blocks[w][5] = back;
    }


    void handle_chunk(shared_ptr<konstructs::Packet> packet) {
        int p, q, k;
        char *pos = packet->buffer();

        p = ntohl(*((int*)pos));
        pos += sizeof(int);

        q = ntohl(*((int*)pos));
        pos += sizeof(int);

        k = ntohl(*((int*)pos));
        pos += sizeof(int);

        Vector3i position(p, q, k);
        const int blocks_size = packet->size - 3 * sizeof(int);
        auto chunk = make_shared<ChunkData>(position, pos, blocks_size);
        chunks.insert({position, chunk});
        model_factory.create_model(position, chunks);
        client.chunk(1);
    }

    void init_menu() {
        using namespace nanogui;

        Window *window = new Window(this, "Button demo");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());
    }

    BlockData blocks;
    Crosshair crosshair;
    ChunkShader chunk;
    ChunkModelFactory model_factory;
    Client client;
    Player player;
    double px;
    double py;
    std::unordered_map<Vector3i, shared_ptr<ChunkData>, matrix_hash<Vector3i>> chunks;
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        {
            nanogui::ref<Konstructs> app = new Konstructs();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
