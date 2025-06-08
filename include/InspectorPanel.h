#ifndef INSPECTORPANEL_H
#define INSPECTORPANEL_H
namespace spark
{
    class SceneManager;
    class GameObject;
    class InspectorPanel
    {
    public:
        InspectorPanel() = default;
        ~InspectorPanel() = default;

        void Render(SceneManager &sceneManager, GameObject *selectedGameObject);

    private:
    };
}

#endif // INSPECTORPANEL_H