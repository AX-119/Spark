#ifndef SCENEGRAPHPANEL_H
#define SCENEGRAPHPANEL_H
namespace spark
{
    class SceneManager;
    class GameObject;
    class SceneGraphPanel
    {
    public:
        SceneGraphPanel() = default;
        ~SceneGraphPanel() = default;
        void Render(SceneManager &sceneManager, GameObject *&selectedGameObject);

    private:
        void RenderGameObjectNode(GameObject *obj, GameObject *&selectedGameObject);
    };
}
#endif // SCENEGRAPHPANEL_H