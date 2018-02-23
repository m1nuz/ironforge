#include <SDL2/SDL_events.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>

#include <core/common.hpp>
#include <core/journal.hpp>
#include <core/game.hpp>
#include <core/assets.hpp>

namespace game {

    namespace input {
        static auto joystick_info(SDL_Joystick *joystick) -> void {
            char guid[64];
            SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guid, sizeof (guid));
            journal::info(journal::_INPUT, "          name: %", SDL_JoystickName(joystick));
            journal::info(journal::_INPUT, "          axes: %", SDL_JoystickNumAxes(joystick));
            journal::info(journal::_INPUT, "         balls: %", SDL_JoystickNumBalls(joystick));
            journal::info(journal::_INPUT, "          hats: %", SDL_JoystickNumHats(joystick));
            journal::info(journal::_INPUT, "       buttons: %", SDL_JoystickNumButtons(joystick));
            journal::info(journal::_INPUT, "   instance id: %", SDL_JoystickInstanceID(joystick));
            journal::info(journal::_INPUT, "          guid: %", guid);
            journal::info(journal::_INPUT, "gamecontroller: %", SDL_IsGameController(SDL_JoystickInstanceID(joystick)) ? "yes" : "no");
        }

        static auto controller_append(int dev) -> bool {
            if (SDL_IsGameController(dev)) {

                auto controller = SDL_GameControllerOpen(dev);
                if (!controller) {
                    journal::error(journal::_INPUT, "Couldn't open controller %: %", dev, SDL_GetError());
                    return false;
                }

                journal::info(journal::_INPUT, "Controller % opened", dev);

                joystick_info(SDL_GameControllerGetJoystick(controller));

                return true;
            }

            const char *name = SDL_JoystickNameForIndex(dev);
            journal::warning(journal::_INPUT, "Unknown controller %", name ? name : "Unknown joystick");

            SDL_Joystick *joystick = SDL_JoystickOpen(dev);
            if (!joystick) {
                journal::error(journal::_INPUT, "SDL_JoystickOpen(%) failed: %", dev, SDL_GetError());
                return false;
            }

            joystick_info(joystick);
            SDL_JoystickClose(joystick);

            return true;
        }

        auto init(instance_t &inst) -> bool {
            auto controller_db = assets::get_text("gamecontrollerdb.txt");
            auto rw = SDL_RWFromMem(controller_db.text, controller_db.size);

            if (!rw)
                return false;

            if (SDL_GameControllerAddMappingsFromRW(rw, 1) == -1) {
                journal::error(journal::_INPUT, "%", SDL_GetError());
                return false;
            }

            return true;
        }

        auto process_event(instance_t &inst, const SDL_Event &ev) -> void {
            (void)inst;

            switch (ev.type) {
            case SDL_JOYDEVICEADDED:
                if (controller_append(ev.jdevice.which))
                    journal::info(journal::_INPUT, "Joystick device % added.", ev.jdevice.which);
                break;
            case SDL_JOYDEVICEREMOVED:
                journal::info(journal::_INPUT, "Joystick device % removed.", ev.jdevice.which);
                break;
            case SDL_CONTROLLERDEVICEADDED:
                journal::info(journal::_INPUT, "Controller device % added.", ev.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                journal::info(journal::_INPUT, "Controller device % added.", ev.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMAPPED:
                journal::info(journal::_INPUT, "Controller device % mapped.", ev.cdevice.which);
                break;
            }
        }

        auto update(instance_t &inst) -> void {
            (void)inst;

            SDL_JoystickUpdate();
            SDL_GameControllerUpdate();
        }
    } // namespace input

} // namespace game
