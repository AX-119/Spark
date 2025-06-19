# Spark!

Spark is a work-in-progress 2D game engine that aims to be a spiritual successor to Adobe Flash. It provides a component-based architecture, scene management, and Lua scripting capabilities to empower developers in creating their own games and interactive experiences.

The main selling point of Spark will be its ease of use, provided by the fact that it will be easily embeddable into webpages due to it being easily transpiled into WASM using Emscripten. 

## ✨ Features

* **Component-Based Architecture:** Build `GameObjects` by attaching and customizing `Components`.
* **Scene Management:** Organize your game with `Scenes` and easily switch between them.
* **Lua Scripting:** Drive game logic and behavior using Lua scripts, with a comprehensive API to interact with the engine.
* **Live Script Reloading:** Reload Lua scripts at runtime to see changes instantly without restarting your application.
* **Immediate Mode GUI with ImGui:** A built-in editor interface, including a scene graph and inspector panel, powered by ImGui.
* **Flexible Rendering:** Utilizes SDL3 for rendering, with a `Renderer` class that abstracts drawing operations.
* **Parent-Child Transformations:** Create complex hierarchies of `GameObjects` with a robust transformation system.

## 🚀 Getting Started

### Prerequisites

* A C++20 compatible compiler (e.g., GCC, Clang, MSVC).
* CMake (version 3.10 or higher).
* Git.

### Building and Running

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/AX-119/Spark.git
    cd spark
    ```

2.  **Configure and build with CMake:**
    ```bash
    cmake -B build
    cmake --build build
    ```

3.  **Run the application:**
    The executable `Spark` will be located in the `build/bin` directory.
    ```bash
    ./build/bin/Spark
    ```

## 🛠️ Dependencies

Spark utilizes the following libraries, which are fetched automatically by CMake using `FetchContent`:

* **SDL3:** For windowing, input, and rendering.
* **ImGui:** For the editor user interface.
* **glm:** For mathematics (vectors, matrices).
* **Lua:** As the scripting language.
* **sol3:** A C++ library binding to Lua.

## 📁 Project Structure

```
.
├── extern/         # External dependencies fetched by CMake
├── include/        
│   ├── Components/ 
│   └── ...
├── res/            # Resources (fonts, scripts, etc.)
└── src/            
    ├── Components/ 
    └── ...
```

## 🔧 Usage

The entry point of the application is in `src/main.cpp`. A default scene is created with a "ScriptRunner" `GameObject` that can be used to load and execute different Lua scripts. The `ScriptSwitcherComponent` allows for easy switching between various example scripts provided in the `res/` directory.

### Creating a GameObject

You can create a new `GameObject` and add components to it like so:

```cpp
auto scene = sceneManager.GetCurrentScene();
    if (scene)
    {
        auto myGameObject = scene->EmplaceGameObject("MyObject");
        myGameObject->AddComponent<MyCustomComponent>();
    }
```

### Lua Scripting

Lua scripts can interact with `GameObjects` and their `Components`. A typical script can have `Init`, `Update`, and `Render` or `RenderImGui` functions:

```lua
-- example.lua
function Init()
    -- Code to run on initialization
end

function Update(dt)
    -- Code to run every frame
end

function Render()
    -- Code to draw to the screen
end
function RenderImGui()
    -- Call ImGui functions here
end
```

The Lua API provides access to engine features such as:

* The owning `GameObject` using the `gameObject` variable
* Getting and setting the position, rotation, and scale of a `GameObject`'s `TransformComponent`.
* Accessing window properties like title, width, and height.
* Drawing shapes and lines with the `Renderer`.
* Getting mouse input.

## 🤝 Contributing

This project is a work in progress, and contributions are welcome. Please feel free to fork the repository, make changes, and submit a pull request.

## 📝 TO DO

Here are some of the planned improvements and features for the future of Spark:

- Splitting the engine into a static/dynamic library: Refactor the project to build the engine as a library that can be linked against, separating it more cleanly from the main application/game.
- Updating the CMake for easier Emscripten compilation: Improve the CMake configuration to streamline the process of compiling the engine and projects to WebAssembly using the Emscripten toolchain.
- Full runtime usability: Enhance the engine's capabilities to allow for adding and removing components, scripts, and resources on-the-fly at runtime, making it more dynamic and flexible.

## 📜 License

The license for this project is not specified in the provided files.