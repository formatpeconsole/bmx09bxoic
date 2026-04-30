#include "utils.h"
#include "binds-manager.h"

namespace gui::binds
{
WPARAM ImGui_ImplWin32_ImGuiKeyToKeyEvent(ImGuiKey imgui_key)
{
    if (imgui_key == ImGuiKey_KeypadEnter)
        return VK_RETURN;

    switch (imgui_key)
    {
    case ImGuiKey_Tab: return VK_TAB;
    case ImGuiKey_LeftArrow: return VK_LEFT;
    case ImGuiKey_RightArrow: return VK_RIGHT;
    case ImGuiKey_UpArrow: return VK_UP;
    case ImGuiKey_DownArrow: return VK_DOWN;
    case ImGuiKey_PageUp: return VK_PRIOR;
    case ImGuiKey_PageDown: return VK_NEXT;
    case ImGuiKey_Home: return VK_HOME;
    case ImGuiKey_End: return VK_END;
    case ImGuiKey_Insert: return VK_INSERT;
    case ImGuiKey_Delete: return VK_DELETE;
    case ImGuiKey_Backspace: return VK_BACK;
    case ImGuiKey_Space: return VK_SPACE;
    case ImGuiKey_Enter: return VK_RETURN;
    case ImGuiKey_Escape: return VK_ESCAPE;
    case ImGuiKey_Comma: return VK_OEM_COMMA;
    case ImGuiKey_Period: return VK_OEM_PERIOD;
    case ImGuiKey_CapsLock: return VK_CAPITAL;
    case ImGuiKey_ScrollLock: return ImGuiKey_ScrollLock;
    case ImGuiKey_NumLock: return VK_NUMLOCK;
    case ImGuiKey_PrintScreen: return VK_SNAPSHOT;
    case ImGuiKey_Pause: return VK_PAUSE;
    case ImGuiKey_Keypad0: return VK_NUMPAD0;
    case ImGuiKey_Keypad1: return VK_NUMPAD1;
    case ImGuiKey_Keypad2: return VK_NUMPAD2;
    case ImGuiKey_Keypad3: return VK_NUMPAD3;
    case ImGuiKey_Keypad4: return VK_NUMPAD4;
    case ImGuiKey_Keypad5: return VK_NUMPAD5;
    case ImGuiKey_Keypad6: return VK_NUMPAD6;
    case ImGuiKey_Keypad7: return VK_NUMPAD7;
    case ImGuiKey_Keypad8: return VK_NUMPAD8;
    case ImGuiKey_Keypad9: return VK_NUMPAD9;
    case ImGuiKey_KeypadDecimal: return VK_DECIMAL;
    case ImGuiKey_KeypadDivide: return VK_DIVIDE;
    case ImGuiKey_KeypadMultiply: return VK_MULTIPLY;
    case ImGuiKey_KeypadSubtract: return VK_SUBTRACT;
    case ImGuiKey_KeypadAdd: return VK_ADD;
    case ImGuiKey_LeftShift: return VK_SHIFT;
    case ImGuiKey_LeftCtrl: return VK_CONTROL;
    case ImGuiKey_LeftAlt: return VK_MENU;
    case ImGuiKey_LeftSuper: return VK_LWIN;
    case ImGuiKey_RightShift: return VK_SHIFT;
    case ImGuiKey_RightCtrl: return VK_CONTROL;
    case ImGuiKey_RightAlt: return VK_MENU;
    case ImGuiKey_RightSuper: return VK_RWIN;
    case ImGuiKey_Menu: return VK_APPS;
    case ImGuiKey_0: return '0';
    case ImGuiKey_1: return '1';
    case ImGuiKey_2: return '2';
    case ImGuiKey_3: return '3';
    case ImGuiKey_4: return '4';
    case ImGuiKey_5: return '5';
    case ImGuiKey_6: return '6';
    case ImGuiKey_7: return '7';
    case ImGuiKey_8: return '8';
    case ImGuiKey_9: return '9';
    case ImGuiKey_A: return 'A';
    case ImGuiKey_B: return 'B';
    case ImGuiKey_C: return 'C';
    case ImGuiKey_D: return 'D';
    case ImGuiKey_E: return 'E';
    case ImGuiKey_F: return 'F';
    case ImGuiKey_G: return 'G';
    case ImGuiKey_H: return 'H';
    case ImGuiKey_I: return 'I';
    case ImGuiKey_J: return 'J';
    case ImGuiKey_K: return 'K';
    case ImGuiKey_L: return 'L';
    case ImGuiKey_M: return 'M';
    case ImGuiKey_N: return 'N';
    case ImGuiKey_O: return 'O';
    case ImGuiKey_P: return 'P';
    case ImGuiKey_Q: return 'Q';
    case ImGuiKey_R: return 'R';
    case ImGuiKey_S: return 'S';
    case ImGuiKey_T: return 'T';
    case ImGuiKey_U: return 'U';
    case ImGuiKey_V: return 'V';
    case ImGuiKey_W: return 'W';
    case ImGuiKey_X: return 'X';
    case ImGuiKey_Y: return 'Y';
    case ImGuiKey_Z: return 'Z';
    case ImGuiKey_F1: return VK_F1;
    case ImGuiKey_F2: return VK_F2;
    case ImGuiKey_F3: return VK_F3;
    case ImGuiKey_F4: return VK_F4;
    case ImGuiKey_F5: return VK_F5;
    case ImGuiKey_F6: return VK_F6;
    case ImGuiKey_F7: return VK_F7;
    case ImGuiKey_F8: return VK_F8;
    case ImGuiKey_F9: return VK_F9;
    case ImGuiKey_F10: return VK_F10;
    case ImGuiKey_F11: return VK_F11;
    case ImGuiKey_F12: return VK_F12;
    case ImGuiKey_F13: return VK_F13;
    case ImGuiKey_F14: return VK_F14;
    case ImGuiKey_F15: return VK_F15;
    case ImGuiKey_F16: return VK_F16;
    case ImGuiKey_F17: return VK_F17;
    case ImGuiKey_F18: return VK_F18;
    case ImGuiKey_F19: return VK_F19;
    case ImGuiKey_F20: return VK_F20;
    case ImGuiKey_F21: return VK_F21;
    case ImGuiKey_F22: return VK_F22;
    case ImGuiKey_F23: return VK_F23;
    case ImGuiKey_F24: return VK_F24;
    case ImGuiKey_AppBack: return VK_BROWSER_BACK;
    case ImGuiKey_AppForward: return VK_BROWSER_FORWARD;
    case ImGuiKey_GraveAccent: return 41;  // `~
    case ImGuiKey_Minus: return 12;         // -
    case ImGuiKey_Equal: return 13;         // =
    case ImGuiKey_LeftBracket: return 26;   // [
    case ImGuiKey_RightBracket: return 27;  // ]
    case ImGuiKey_Oem102: return 86;        // < > \ (102-key)
    case ImGuiKey_Backslash: return 43;     // '\'
    case ImGuiKey_Semicolon: return 39;     // ;
    case ImGuiKey_Apostrophe: return 40;    // '
    case ImGuiKey_Slash: return 53;         // /

    default: break;
    }

    return 0; // Invalid key
}

std::string ImGui_ImplWin32_VKeyToString(int wParam)
{
    switch (wParam)
    {
    case VK_LBUTTON: return "M1";
    case VK_RBUTTON: return "M2";
    case VK_CANCEL: return "Cancel";
    case VK_MBUTTON: return "M3";
    case VK_XBUTTON1: return "M4";
    case VK_XBUTTON2: return "M5";
    case VK_BACK: return "Backspace";
    case VK_TAB: return "Tab";
    case VK_CLEAR: return "Clear";
    case VK_RETURN: return "Enter";
    case VK_SHIFT: return "Shift";
    case VK_CONTROL: return "Ctrl";
    case VK_MENU: return "Alt";
    case VK_PAUSE: return "Pause";
    case VK_CAPITAL: return "CapsLock";
    case VK_KANA: return "Kana";
    case VK_JUNJA: return "Junja";
    case VK_FINAL: return "Final";
    case VK_HANJA: return "Hanja";
    case VK_ESCAPE: return "Escape";
    case VK_CONVERT: return "Convert";
    case VK_NONCONVERT: return "NonConvert";
    case VK_ACCEPT: return "Accept";
    case VK_MODECHANGE: return "ModeChange";
    case VK_SPACE: return "Space";
    case VK_PRIOR: return "PGUP";
    case VK_NEXT: return "PGDOWN";
    case VK_END: return "End";
    case VK_HOME: return "Home";
    case VK_LEFT: return "Left";
    case VK_UP: return "Up";
    case VK_RIGHT: return "Right";
    case VK_DOWN: return "Down";
    case VK_SELECT: return "Select";
    case VK_PRINT: return "Print";
    case VK_EXECUTE: return "Execute";
    case VK_SNAPSHOT: return "PrintScreen";
    case VK_INSERT: return "Insert";
    case VK_DELETE: return "Delete";
    case VK_HELP: return "Help";
    case VK_LWIN: return "LeftSuper";
    case VK_RWIN: return "RightSuper";
    case VK_APPS: return "Menu";
    case VK_SLEEP: return "Sleep";
    case VK_NUMPAD0: return "Num 0";
    case VK_NUMPAD1: return "Num 1";
    case VK_NUMPAD2: return "Num 2";
    case VK_NUMPAD3: return "Num 3";
    case VK_NUMPAD4: return "Num 4";
    case VK_NUMPAD5: return "Num 5";
    case VK_NUMPAD6: return "Num 6";
    case VK_NUMPAD7: return "Num 7";
    case VK_NUMPAD8: return "Num 8";
    case VK_NUMPAD9: return "Num 9";
    case VK_MULTIPLY: return "Num *";
    case VK_ADD: return "Num +";
    case VK_SEPARATOR: return "Num -";
    case VK_SUBTRACT: return "Num -";
    case VK_DECIMAL: return "Num .";
    case VK_DIVIDE: return "Num /";
    case VK_F1: return "F1";
    case VK_F2: return "F2";
    case VK_F3: return "F3";
    case VK_F4: return "F4";
    case VK_F5: return "F5";
    case VK_F6: return "F6";
    case VK_F7: return "F7";
    case VK_F8: return "F8";
    case VK_F9: return "F9";
    case VK_F10: return "F10";
    case VK_F11: return "F11";
    case VK_F12: return "F12";
    case VK_F13: return "F13";
    case VK_F14: return "F14";
    case VK_F15: return "F15";
    case VK_F16: return "F16";
    case VK_F17: return "F17";
    case VK_F18: return "F18";
    case VK_F19: return "F19";
    case VK_F20: return "F20";
    case VK_F21: return "F21";
    case VK_F22: return "F22";
    case VK_F23: return "F23";
    case VK_F24: return "F24";
    case VK_NUMLOCK: return "NumLock";
    case VK_SCROLL: return "ScrollLock";
    case VK_LSHIFT: return "LeftShift";
    case VK_RSHIFT: return "RightShift";
    case VK_LCONTROL: return "LeftCtrl";
    case VK_RCONTROL: return "RightCtrl";
    case VK_LMENU: return "LeftAlt";
    case VK_RMENU: return "RightAlt";
    case VK_BROWSER_BACK: return "AppBack";
    case VK_BROWSER_FORWARD: return "AppForward";
    case VK_BROWSER_REFRESH: return "BrowserRefresh";
    case VK_BROWSER_STOP: return "BrowserStop";
    case VK_BROWSER_SEARCH: return "BrowserSearch";
    case VK_BROWSER_FAVORITES: return "BrowserFavorites";
    case VK_BROWSER_HOME: return "BrowserHome";
    case VK_VOLUME_MUTE: return "VolumeMute";
    case VK_VOLUME_DOWN: return "VolumeDown";
    case VK_VOLUME_UP: return "VolumeUp";
    case VK_MEDIA_NEXT_TRACK: return "MediaNext";
    case VK_MEDIA_PREV_TRACK: return "MediaPrev";
    case VK_MEDIA_STOP: return "MediaStop";
    case VK_MEDIA_PLAY_PAUSE: return "MediaPlayPause";
    case VK_LAUNCH_MAIL: return "LaunchMail";
    case VK_LAUNCH_MEDIA_SELECT: return "LaunchMedia";
    case VK_LAUNCH_APP1: return "LaunchApp1";
    case VK_LAUNCH_APP2: return "LaunchApp2";

    case VK_OEM_1: return ";";      // ;:/
    case VK_OEM_PLUS: return "+"; // =+
    case VK_OEM_COMMA: return ","; // ,<
    case VK_OEM_MINUS: return "-"; // -_
    case VK_OEM_PERIOD: return "."; // .>
    case VK_OEM_2: return "/";      // /?
    case VK_OEM_3: return "`";      // `~
    case VK_OEM_4: return "[";      // [{ 
    case VK_OEM_5: return "|";      // \|
    case VK_OEM_6: return "]";      // ]}
    case VK_OEM_7: return "'";      // '"
    case VK_OEM_8: return "OEM_8";
    case VK_OEM_AX: return "OEM_AX";
    case VK_OEM_102: return "OEM_102";
    case VK_ICO_HELP: return "ICO_HELP";
    case VK_ICO_00: return "ICO_00";

    case '0': return "0";
    case '1': return "1";
    case '2': return "2";
    case '3': return "3";
    case '4': return "4";
    case '5': return "5";
    case '6': return "6";
    case '7': return "7";
    case '8': return "8";
    case '9': return "9";

    case 'A': return "A";
    case 'B': return "B";
    case 'C': return "C";
    case 'D': return "D";
    case 'E': return "E";
    case 'F': return "F";
    case 'G': return "G";
    case 'H': return "H";
    case 'I': return "I";
    case 'J': return "J";
    case 'K': return "K";
    case 'L': return "L";
    case 'M': return "M";
    case 'N': return "N";
    case 'O': return "O";
    case 'P': return "P";
    case 'Q': return "Q";
    case 'R': return "R";
    case 'S': return "S";
    case 'T': return "T";
    case 'U': return "U";
    case 'V': return "V";
    case 'W': return "W";
    case 'X': return "X";
    case 'Y': return "Y";
    case 'Z': return "Z";

    default:
        return std::to_string(wParam);
        break;
    }
}

std::string getItemType(int type)
{
    switch (type)
    {
    case ITEM_CHECKBOX:
        return "Checkbox";
    case ITEM_SLIDER_INT:
        return "Slider<int>";
    case ITEM_SLIDER_FLOAT:
        return "Slider<float>";
    case ITEM_COMBOBOX:
        return "ComboBox";
    case ITEM_MULTICOMBOBOX:
        return "MultiComboBox";
    case ITEM_COLOR:
        return "Color";
    default:
        return "";
    }
}

std::string getBindMode(int mode)
{
    switch (mode)
    {
    case BIND_ALWAYS_ON:
        return "Always On";
    case BIND_HOLD:
        return "Hold";
    case BIND_TOGGLE:
        return "Toggle";
    case BIND_RELEASE:
        return "Release";
    case BIND_FORCE_OFF:
        return "Force Off";
    default:
        return "";
    }
}
}