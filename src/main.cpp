#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <vector>
#include <cmath>

using namespace geode::prelude;

static constexpr int BLACK_CHANNEL = 1010; // GD's built-in Black color
static constexpr int SOLID_ID = 1;         // plain solid block used as the black fill
static constexpr float DEPTH = 4.f;        // 3D offset, up and right

class $modify(AutoDecoUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        auto spr = ButtonSprite::create("Deco");
        spr->setScale(0.7f);
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(AutoDecoUI::onAutoDeco)
        );
        btn->setID("auto-deco-button"_spr);

        auto menu = CCMenu::create();
        menu->setID("auto-deco-menu"_spr);
        menu->setPosition({0.f, 0.f});
        menu->addChild(btn);

        auto win = CCDirector::get()->getWinSize();
        btn->setPosition({win.width * 0.32f, win.height * 0.94f});

        this->addChild(menu, 100);
        return true;
    }

    GameObject* spawn(int id, GameObject* src, CCPoint pos, bool black, int z) {
        auto o = m_editorLayer->createObject(id, pos, true);
        if (!o) return nullptr;
        o->setRotation(src->getRotation());
        o->setScaleX(src->getScaleX());
        o->setScaleY(src->getScaleY());
        if (black) {
            if (o->m_baseColor) o->m_baseColor->m_colorID = BLACK_CHANNEL;
            if (o->m_detailColor) o->m_detailColor->m_colorID = BLACK_CHANNEL;
        }
        o->m_zOrder = z;
        return o;
    }

    void onAutoDeco(CCObject*) {
        auto selected = this->getSelectedObjects();
        if (!selected || selected->count() == 0) {
            FLAlertLayer::create("Auto Deco", "Select some blocks first.", "OK")->show();
            return;
        }

        std::vector<GameObject*> objs;
        for (auto obj : CCArrayExt<GameObject*>(selected)) objs.push_back(obj);

        int added = 0;
        for (auto obj : objs) {
            auto p = obj->getPosition();
            auto shifted = CCPoint{p.x + DEPTH, p.y + DEPTH};
            int z = obj->m_zOrder;

            // back layer: white outline + black fill, shifted up-right (the 3D sides)
            if (spawn(obj->m_objectID, obj, shifted, false, z - 4)) added++;
            if (spawn(SOLID_ID, obj, shifted, true, z - 3)) added++;
            // front layer: black fill right behind your outline
            if (spawn(SOLID_ID, obj, p, true, z - 2)) added++;
        }

        FLAlertLayer::create(
            "Auto Deco", fmt::format("Added {} objects.", added), "OK"
        )->show();
    }
};
