/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 2014, 2021 Daniel Potter <dmpotter44@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#ifndef __CONTROL_HPP_
#define __CONTROL_HPP_

#include "SDL.h"

// SDL seems to only support up to 5 buttons but it returns a 32-bit mask of
// buttons so we may as well support up to 32. Some MMO mouses have nearly 20
// buttons on them, so who knows.
#define MAX_MOUSE_BUTTONS       32

/**
 * Effectively an interface for receiving notifications of control changes.
 */
class ControlBinding {
public:
    /**
     * Some source of this binding has changed to the given state.
     * \param state true when active, false when inactive
     */
    virtual void OnControlChange(bool state) = 0;
};

/**
 * Parses a key definition to lookup a scancode. This goes through several
 * steps before determining a final scancode. This method is not case sensitive
 * (as SDL_GetScancodeFromName is not case sensitive).
 *
 * If the string is a single character long, it's immediately passed off to
 * SDL_GetScancodeFromName to determine the key.
 *
 * If the keyname can be parsed by strtol (using base 0, meaning the 0x and 0
 * prefixes are allowed for hexidecimal and octal respectively), that's used
 * immediately as a scancode and returned (assuming it's less than
 * SDL_NUM_SCANCODES, if it's greater than or equal to, it returns
 * SDL_SCANCODE_UNKNOWN).
 *
 * If the keyname starts with "scancode", it is taken as a scancode. (This
 * is primarily to allow scancode 0-9, which otherwise would become keys
 * 0-9.) It's parsed using the above method. (As an example of why you could
 * want to do this, to target WASD controls that remain WASD regardless of
 * layout, you can use "scancode 4" for A and "scancode 7" for D.)
 *
 * Otherwise, the keyname is passed to SDL_GetScancodeFromName as-is to
 * determine a final scancode, which will result in SDL_SCANCODE_UNKNOWN if
 * it cannot parse the keycode name.
 */
SDL_Scancode ParseScancode(const char* keyname);

/**
 * Control bindings. Deals with directing SDL events to bound controls.
 * Note that it is the responsibility of the caller to deal with creating and
 * destroying ControlBindings, this class merely handles dispatching to them.
 */
class Bindings {
public:
    Bindings();
    ~Bindings();
    /**
     * Removes all key bindings.
     */
    void ResetBindings();
    /**
     * Fire an event.
     */
    void FireEvent(const SDL_Event& event);
    /**
     * Binds the given callback to a given key. The same callback may be bound
     * to multiple events. The callback may be NULL in which case the key is
     * unbound.
     */
    int BindKey(SDL_Scancode scancode, ControlBinding* binding);
    /**
     * Binds a key based on its name. Uses ParseKeyName to parse the key name.
     */
    int BindKeyByName(const char* keyname, ControlBinding* binding);
    /**
     * Binds a key based on its name. Otherwise identical to BindKey.
     * The scancode is looked up using SDL_GetKeyFromName followed by
     * SDL_GetScancodeFromKey.
     */
    int BindKeyByKeyName(const char* keyname, ControlBinding* binding);
    /**
     * Binds a key based on its name. Otherwise identical to BindKey.
     * The scancode is looked up using SDL_GetScancodeFromName.
     */
    int BindKeyByScancodeName(const char* keyname, ControlBinding* binding);
    /**
     * Remove the binding for a given key.
     */
    int UnbindKey(SDL_Scancode scancode);
    /**
     * Binds the given callback to a given mouse button. The same callback may
     * be bound to multiple events. The callback may be NULL in which case
     * the mouse button is unbound.
     */
    int BindMouseButton(Uint8 mouseButton, ControlBinding* binding);
private:
    ControlBinding* m_key_bindings[SDL_NUM_SCANCODES];
    ControlBinding* m_mouse_bindings[MAX_MOUSE_BUTTONS];
    // The bindings for controllers gets a bit weird.
};

#endif // __CONTROL_HPP_
