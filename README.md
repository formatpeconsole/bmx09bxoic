# BMX09BXOIC

Simple base for CS2.

### Pages
  - [Key features](#key-features)
  - [Get Started](#get-started)
  - [P.S.](#ps)

### Key features
  - [Multi-Bind System](#multi-bind-system)
  - [Thread Pool](#thread-pool)

# Multi-Bind System
Multi-Bind system allows users to customize any item by binding any amount of binds with different types and keys

**Main goal** is to achieve handling of UI items in different UI frameworks (separate bind logic & ui items logic)

How it looks (using ImGui for example)

![](https://i.imgur.com/2Yz3AIO.jpeg)
![](https://i.imgur.com/OkBlakE.png)

### Benefits:
- Key-Locking system. When user adds multiple binds on same item and tries to press different buttons - it will lock other binds except pressed one on this item till user stops pressing main bind.
- Customization. You are not limited in any use-cases of it. Just use binds however you want!

### Usage:
- Initializing
```cpp
// First you have to create object of KeyBindManager
// In my case it's in Menu struct:
KeyBindManager keyBindManager;

// Then you have to init this manager at your Main:
getMenuInstance().keyBindManager.init();
```

- Initializing key handling
```cpp
// In your WNDProc function you have to call handleMainBinds and updateMainBindStates
// As it shown in example:
LRESULT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    gui::binds::handleMainBinds(uMsg, wParam, lParam);
    getMenuInstance().keyBindManager.updateMainBindStates();
    return CallWindowProcA(getRenderInfoInstance().wndProcOrig, hwnd, uMsg, wParam, lParam);
}

// In your render loop you have to also call other functions for make this system work:
HRESULT Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    // handles all new values that were recorded to UI 
    getMenuInstance().keyBindManager.updateAliveBindValues();

    // handles all bind states that won't react on key (Always On / Release / Force Off)
    getMenuInstance().keyBindManager.updateOtherBindStates();

    // handles other binds again due to delays in WndProc
    // makes it much safe
    getMenuInstance().keyBindManager.updateMainBindStates();

    return PresentCall(pSwapChain, SyncInterval, Flags);
}
```

- Destroying
```cpp
// Put this function at the end of your main / finish of program
getMenuInstance().keyBindManager.destroy();
```

- Add item to bind
```cpp
// In this example i added opening of UI by insert in this system
// First you have to record bind by using addBind method:

// Args are:
// ptr - pointer to your item value (in this case it's pointer too boolean for ui open)
//
// bindPtr - pointer to your bind item value
//
// oldValue - pointer to storage of previous value on bind after it gets disabled
//
// type - type of your bind:
//
//   - BIND_NONE = default, just to skip empty binds in future
//   - BIND_ALWAYS_ON
//   - BIND_HOLD
//   - BIND_TOGGLE
//   - BIND_RELEASE
//   - BIND_FORCE_OFF
//
// itemType - type of your item depends on your UI 
// in my case types are:
//
//   - ITEM_NONE = same reason as for BIND_NONE
//   - ITEM_UI_OPEN
//   - ITEM_CHECKBOX
//   - ITEM_SLIDER_INT
//   - ITEM_SLIDER_FLOAT
//   - ITEM_COMBOBOX
//   - ITEM_MULTICOMBOBOX
//   - ITEM_COLOR
//
// name - name of bind for using it in UI or Configs
// for UI open button it's empty because it won't be visualized
//
// itemName - name of your item in UI (it can be used for visualizing binds as i did in the example)
// for UI open button it's empty for the same reason as above
getMenuInstance().keyBindManager.addBind(
    &getMenuInstance().opened,
    &getMenuInstance().newOpened,
    &getMenuInstance().openedOldValue,
    BIND_TOGGLE,
    ITEM_UI_OPEN,
    VK_INSERT,
    {},
    {}
 );

// after you added your bind you HAVE to add to updating new value
// that should be used for overriding bind
// in this case value will change from true to false
// so new value will be always TRUE
// for chaning values in real-time you have to call updateAliveBindValues in your render loop
auto& bindList = getMenuInstance().keyBindManager.getBindList();
const auto menuKey = std::find_if(bindList.begin(), bindList.end(), [](const std::shared_ptr<IKeyBind>& keyBind)
    {
        return keyBind->getKey() == VK_INSERT;
    });

if (menuKey != bindList.end())
{
    auto bindToUpdate = (*menuKey);
    bindToUpdate->setOldValue();
    bindToUpdate->updateNewValue();
}
 ```
- Now you can use this system however you want :)

# Thread Pool
Simple thread pool is used to call heavy operations in multiple threads that boosts perfomance

Don't forget that you have to implement correct thread-safe logic for it 

### Usage:
- Initializing
```cpp
// Just call this function in your main
getSimpleThreadPoolInstance().init();
```
- Destroying
```cpp
// Call when your application gets closed
getSimpleThreadPoolInstance().destroy();
```
- Add tasks
```cpp
 for (int i = 0; i < 100; i++)
 {
     getSimpleThreadPoolInstance().addTask([i]() {
         std::cout << "Hello World! Thread: " << std::this_thread::get_id() << " i: " << i << std::endl;

         std::this_thread::sleep_for(250ms);
     });
 }

 getSimpleThreadPoolInstance().waitWhenReady();
 ```

# Get Started
To use this source you have to install dependencies by using vcpkg (https://github.com/microsoft/vcpkg): 

```bash
vcpkg install --triplet-x64-windows-static
```

# P.S.
(C) Made by miraclestar [19.04.2026-26.04.2026]
