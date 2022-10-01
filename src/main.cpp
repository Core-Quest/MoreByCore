#include "main.hpp"
#include "MainConfig.hpp"
#include "questui/shared/QuestUI.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "config-utils/shared/config-utils.hpp"
#include "UnityEngine/Color.hpp"
#include "GlobalNamespace/GameplayLevelSceneTransitionEvents.hpp"
#include "GlobalNamespace/LightWithIdManager.hpp"
#include "GlobalNamespace/MenuLightsManager.hpp"
#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/IReadonlyBeatmapData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapData.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "GlobalNamespace/ScoreModel.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/LevelSearchViewController.hpp"
#include "GlobalNamespace/GameplaySetupViewController.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "GlobalNamespace/LevelCollectionViewController.hpp"
#include "GlobalNamespace/SongController.hpp"
#include "System/Action.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "GlobalNamespace/HealthWarningViewController.hpp"
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace custom_types;
using namespace QuestUI::BeatSaberUI;


static ModInfo modInfo;
/// Define Our Config
DEFINE_CONFIG(MainConfig);

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}
float preview_acc;
bool updatelights;
GlobalNamespace::MenuLightsManager *menuLightsmanager;

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedtoHierarchy, bool screenSystemEnabling) {
    using namespace UnityEngine;

    if(firstActivation)
    {
        GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        UnityEngine::UI::VerticalLayoutGroup* Vertical = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(self->get_transform());
        UnityEngine::UI::HorizontalLayoutGroup* Horizontal = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(self->get_transform());

        QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), "Made By Core");
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "");
        QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), "Menu/(In)Game Settings");
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "_______________________________________________");
        AddConfigValueToggle(container->get_transform(), getMainConfig().isSaberMarqEnabled);
        AddConfigValueToggle(container->get_transform(), getMainConfig().NoPromo);
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "_______________________________________________");
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "");
        QuestUI::BeatSaberUI::CreateUIButton(container->get_transform(), "Menu Light Config");
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "_______________________________________________");
        AddConfigValueModifierButton(container->get_transform(), getMainConfig().lightsEnable);

        

        auto colorPicker = BeatSaberUI::CreateColorPicker(container->get_transform(), "Light Color Changer", getMainConfig().MenuClr.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().MenuClr.SetValue(color, true);
            preview_acc = 0;
            (getMainConfig().updatelights_preview.SetValue(true)); 
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker->get_transform(), "Refresh",UnityEngine::Vector2(0,+0.1),UnityEngine::Vector2(30,8),
        [colorPicker]()
            {
                getMainConfig().MenuClr.SetValue(UnityEngine::Color(5.0, 5.0, 5.0, 5.0));
                colorPicker->set_currentColor(getMainConfig().MenuClr.GetValue());
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
                preview_acc = 0;
            });
        
        UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "*Note: Refresh Resets Colors/Enables(Disables)!");
    }
}





MAKE_HOOK_MATCH(aasd_SaberBurnMarkArea_OnEnable, &GlobalNamespace::SaberBurnMarkArea::OnEnable, void, GlobalNamespace::SaberBurnMarkArea*self)
{
    if(getMainConfig().isSaberMarqEnabled.GetValue() == true){
        int value = 0;
        CRASH_UNLESS(il2cpp_utils::SetFieldValue(self, "_fadeOutStrengthShaderPropertyID", value));
        aasd_SaberBurnMarkArea_OnEnable(self);
        getLogger().info("SaberBurnMarkArea OnEnable Was Called!");
    }
    else if (getMainConfig().isSaberMarqEnabled.GetValue() == false){
        getLogger().info("SaberBurnMarkArea Not Enabled!");
    }
}

MAKE_HOOK_MATCH(AdvertisementButton, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController*self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {

    AdvertisementButton(self, firstActivation, addedToHierarchy, screenSystemEnabling); 
    
    
    UnityEngine::UI::Button*PromotionButton = self->dyn__musicPackPromoButton();
    UnityEngine::GameObject *gameObject = PromotionButton->get_gameObject();
    
    if(getMainConfig().NoPromo.GetValue() == true){
        gameObject->SetActive(false);
        
        getLogger().info("Saber Marqs Turned off promo");
    }
    else if (getMainConfig().NoPromo.GetValue() == false){
        gameObject->SetActive(true);
    }    
    
}

MAKE_HOOK_MATCH(ResultsScreenUI_didactivate, &ResultsViewController::DidActivate, void, GlobalNamespace::ResultsViewController*self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    ResultsScreenUI_didactivate( self, firstActivation, addedToHierarchy, screenSystemEnabling);
    updatelights = true;
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
}
MAKE_HOOK_MATCH(health_and_safety, &HealthWarningViewController::DidActivate, void, GlobalNamespace::HealthWarningViewController *self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    health_and_safety( self, firstActivation, addedToHierarchy, screenSystemEnabling);
    if (getMainConfig().lightsEnable.GetValue() == true){
        updatelights = true;
    }
    else if (getMainConfig().lightsEnable.GetValue() == false){
        getLogger().info("Off");
    }
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
}



MAKE_HOOK_MATCH(ResultsScreenUI_init, &ResultsViewController::Init, void, ResultsViewController* self, LevelCompletionResults* levelCompletionResults, IReadonlyBeatmapData* transformedBeatmapData,
IDifficultyBeatmap* difficultyBeatmap, bool practice, bool newHighScore){
    ResultsScreenUI_init(self,levelCompletionResults,transformedBeatmapData,difficultyBeatmap,practice,newHighScore);

    if (getMainConfig().lightsEnable.GetValue() == true){
        updatelights = true;
    }
    else if (getMainConfig().lightsEnable.GetValue() == false){
        getLogger().info("Off");
    }
    getLogger().info("result screen song complete %d", updatelights);
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
    if (levelCompletionResults != 0){
        UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
    }
}



MAKE_HOOK_MATCH(Song_select, &LevelCollectionViewController::DidActivate, void , LevelCollectionViewController *self , bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    Song_select( self, firstActivation, addedToHierarchy, screenSystemEnabling);
    getLogger().info("song select open");
    if (getMainConfig().lightsEnable.GetValue() == true){
        updatelights = true;
    }
    else if (getMainConfig().lightsEnable.GetValue() == false){
        getLogger().info("Off");
    }
  
}



MAKE_HOOK_MATCH(Song_select_exit, &LevelCollectionViewController::DidDeactivate, void, LevelCollectionViewController *self,bool removedFromHierarchy, bool screenSystemDisabling){
    Song_select_exit( self, removedFromHierarchy, screenSystemDisabling);
    getLogger().info("leaving main menu");
    if (getMainConfig().lightsEnable.GetValue() == true){
        updatelights = true;
    }
    else if (getMainConfig().lightsEnable.GetValue() == false){
        getLogger().info("Off");
    }
}



MAKE_HOOK_MATCH(Main_menu, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController*self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
Main_menu( self, firstActivation, addedToHierarchy, screenSystemEnabling);
getLogger().info("main menu");
    if (getMainConfig().lightsEnable.GetValue() == true){
        updatelights = true;
    }
    else if (getMainConfig().lightsEnable.GetValue() == false){
        getLogger().info("Off");
    }
    (getMainConfig().updatelights_preview.SetValue(false));
    UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
    getLogger().info("Main menu %d", updatelights);
}

MAKE_HOOK_MATCH(LightsUpdater, &LightWithIdManager::SetColorForId, void, LightWithIdManager *self, int lightId, UnityEngine::Color color){

    if (updatelights == true){        
        if (getMainConfig().lightsEnable.GetValue() == true){
            color = getMainConfig().MenuClr.GetValue(); 
        }
        else if (getMainConfig().lightsEnable.GetValue() == false){
            getLogger().info("Lights Not Enabled!");
        }                   
    }
    else if (updatelights == false){
        getLogger().info("Lights off on scene/Mod Off!");
    }
        // preview the lights in the color select screen
    if (getMainConfig().updatelights_preview.GetValue() == true){
        getLogger().info("updating lights");
        
        
        color = getMainConfig().MenuClr.GetValue();
            
        
    }
    else if (getMainConfig().updatelights_preview.GetValue() == false){
        getLogger().info("Not Enabled!");
    }
    LightsUpdater(self, lightId, color);
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();
    
    getMainConfig().Init(modInfo);
    custom_types::Register::AutoRegister();
    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);
    QuestUI::Register::RegisterMainMenuModSettingsViewController(modInfo, DidActivate);
    getLogger().info("Installing hooks...");
    // Install our hooks (none defined yet)
    INSTALL_HOOK(getLogger(), aasd_SaberBurnMarkArea_OnEnable);
    INSTALL_HOOK(getLogger(), AdvertisementButton);
    INSTALL_HOOK(getLogger(), Main_menu)
    INSTALL_HOOK(getLogger(), Song_select)
    INSTALL_HOOK(getLogger(), ResultsScreenUI_init);
    INSTALL_HOOK(getLogger(), LightsUpdater);
    INSTALL_HOOK(getLogger(), Song_select_exit);
    INSTALL_HOOK(getLogger(), ResultsScreenUI_didactivate);
    getLogger().info("Installed all hooks!");
}