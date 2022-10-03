#include "main.hpp"
using namespace MainUI;
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
#include "TMPro/TextAlignmentOptions.hpp"
#include "UnityEngine/TextAnchor.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "System/Type.hpp"
#include "HMUI/ImageView.hpp"
#include "GlobalNamespace/GameCoreSceneSetup.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/ColorSchemeColorToggleController.hpp"
#include "GlobalNamespace/PlayerDataModel.hpp"
#include "GlobalNamespace/PlayerData.hpp"
#include "GlobalNamespace/ColorSchemesSettings.hpp"

using namespace GlobalNamespace;
using namespace QuestUI;
using namespace custom_types;
using namespace QuestUI::BeatSaberUI;

#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/RectOffset.hpp"
#include "GlobalNamespace/BombExplosionEffect.hpp"
#include "GlobalNamespace/NoteCutParticlesEffect.hpp"
#include "GlobalNamespace/PauseMenuManager.hpp"
#include "GlobalNamespace/BombNoteController.hpp"
#include "GlobalNamespace/MusicPackPromoBanner.hpp"
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
using namespace UnityEngine;



int getFontStyleValue(Il2CppObject* self){
	int fontStyle = CRASH_UNLESS(il2cpp_utils::GetPropertyValue<int>(self, "fontStyle"));
	if (fontStyle & (1 << 1)) {
		return fontStyle - 2;
	}
	else {
		return -1;
	}
}

DEFINE_TYPE(MainUI, MoreMainUI);
GameObject *mainCont;
GameObject *saberCont;
GameObject *colorCont;
GameObject *pauseClrCont;

bool screensEnabled = false;
void MoreMainUI::DidActivate(bool firstActivation, bool addedtoHierarchy, bool screenSystemEnabling) {
    using namespace UnityEngine;
    using namespace TMPro;
    using namespace UnityEngine::UI;

    if(firstActivation)
    {
        mainCont = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(get_transform());
        saberCont = QuestUI::BeatSaberUI::CreateFloatingScreen(Vector2(80.0f, 100.f), Vector3(-3.5f, 1.0f, 3.5f), Vector3(0.0f, -40.0f, 0.0f), 0.0f, true, false);
        colorCont = QuestUI::BeatSaberUI::CreateFloatingScreen(Vector2(80.0f, 100.f), Vector3(3.5f, 1.0f, 3.5f), Vector3(0.0f, 40.0f, 0.0f), 0.0f, true, false);
        UnityEngine::UI::VerticalLayoutGroup* vertClrCont = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(colorCont->get_transform());

        





        QuestUI::BeatSaberUI::CreateText(mainCont->get_transform(), "");
        QuestUI::BeatSaberUI::CreateUIButton(mainCont->get_transform(), "Main Configs", UnityEngine::Vector2(0.0f, 90.0f),UnityEngine::Vector2(79.0f, 10.0f));

        AddConfigValueToggle(mainCont->get_transform(), getMainConfig().NoPromo);
        AddConfigValueToggle(mainCont->get_transform(), getMainConfig().NoParticles);
        AddConfigValueToggle(mainCont->get_transform(), getMainConfig().NoBombs);



        QuestUI::BeatSaberUI::CreateUIButton(saberCont->get_transform(), "Saber Configs", UnityEngine::Vector2(0.0f, 54.0f),UnityEngine::Vector2(79.9f, 10.0f));
        AddConfigValueToggle(saberCont->get_transform(), getMainConfig().isSaberMarqEnabled);
        



        QuestUI::BeatSaberUI::CreateUIButton(colorCont->get_transform(), "Menu Light Config", UnityEngine::Vector2(0.0f, 54.0f),UnityEngine::Vector2(79.9f, 10.0f));
        AddConfigValueToggle(colorCont->get_transform(), getMainConfig().lightsEnable);

        

        auto colorPicker = BeatSaberUI::CreateColorPicker(colorCont->get_transform(), "Light Color", getMainConfig().MenuClr.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().MenuClr.SetValue(color, true);
            preview_acc = 0;
            (getMainConfig().updatelights_preview.SetValue(true)); 
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker->get_transform(), "Refresh",UnityEngine::Vector2(0,+0.3),UnityEngine::Vector2(30,8),
        [colorPicker]()
            {
                getMainConfig().MenuClr.SetValue(UnityEngine::Color(5.0, 5.0, 5.0, 5.0));
                colorPicker->set_currentColor(getMainConfig().MenuClr.GetValue());
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
                preview_acc = 0;
            });
        
        UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
        QuestUI::BeatSaberUI::CreateText(colorCont->get_transform(), "*Note: Refresh Resets Colors/Enables(Disables)!");
        
        

    }
    if (!screensEnabled) {
        screensEnabled = true;
        mainCont->SetActive(true);
        saberCont->SetActive(true);
        colorCont->SetActive(true);
 
        
    }
}

void MoreMainUI::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling) {
    screensEnabled = false;
    mainCont->SetActive(false);
    saberCont->SetActive(false);
    colorCont->SetActive(false);
 
   

}

MAKE_HOOK_MATCH(PauseMenuManager_ShowMenu, &PauseMenuManager::ShowMenu, void, PauseMenuManager *self){
    PauseMenuManager_ShowMenu(self);

    if (getMainConfig().lightsEnable.GetValue()){
        pauseClrCont = BeatSaberUI::CreateFloatingScreen(Vector2(70.0f, 70.0f), Vector3(1.0f, 2.85f, 2.4f), Vector3(-30.0f, 0.0f, 0.0f), 0.0f, false, false);

        auto colorPicker = BeatSaberUI::CreateColorPicker(pauseClrCont->get_transform(), "Light Color", getMainConfig().MenuClr.GetValue(),[](UnityEngine::Color color) {
            getMainConfig().MenuClr.SetValue(color, true);
            preview_acc = 0;
            (getMainConfig().updatelights_preview.SetValue(true)); 
            UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
            (getMainConfig().updatelights_preview.SetValue(false));
        });

        QuestUI::BeatSaberUI::CreateUIButton(colorPicker->get_transform(), "Refresh",UnityEngine::Vector2(0,+0.3),UnityEngine::Vector2(20,8),
        [colorPicker]()
            {
                getMainConfig().MenuClr.SetValue(UnityEngine::Color(5.0, 5.0, 5.0, 5.0));
                colorPicker->set_currentColor(getMainConfig().MenuClr.GetValue());
                UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
                preview_acc = 0;
            });
        
        UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuLightsManager*>().First()->RefreshColors();
        if (!screensEnabled)
        {
        getLogger().info("Setting screen to active");
        pauseClrCont->SetActive(true);
        }
    }
}

MAKE_HOOK_MATCH(PauseMenuManager_ContinueButtonPressed, &PauseMenuManager::ContinueButtonPressed, void, PauseMenuManager *self) {
    PauseMenuManager_ContinueButtonPressed(self);
    
    if (!getMainConfig().lightsEnable.GetValue())
    {
    getLogger().info("Disabling Color Screen!");
    pauseClrCont->SetActive(false);
    }
    else if (getMainConfig().lightsEnable.GetValue()) {}
}
MAKE_HOOK_MATCH(NoteCutParticlesEffect_SpawnParticles, &GlobalNamespace::NoteCutParticlesEffect::SpawnParticles, void, GlobalNamespace::NoteCutParticlesEffect* self, UnityEngine::Vector3 cutPoint, UnityEngine::Vector3 cutNormal, UnityEngine::Vector3 saberDir, float saberSpeed, UnityEngine::Vector3 noteMovementVec, UnityEngine::Color32 color, int sparkleParticlesCount, int explosionParticlesCount, float LifetimeMultiplier
){

    if(getMainConfig().NoParticles.GetValue())
    {
        sparkleParticlesCount = 0;
        explosionParticlesCount = 0;
        NoteCutParticlesEffect_SpawnParticles(self, cutPoint, cutNormal, saberDir, saberSpeed, noteMovementVec, color, sparkleParticlesCount,explosionParticlesCount,LifetimeMultiplier);
    }
    NoteCutParticlesEffect_SpawnParticles(self, cutPoint, cutNormal, saberDir, saberSpeed, noteMovementVec, color, sparkleParticlesCount,explosionParticlesCount,LifetimeMultiplier);

};

MAKE_HOOK_MATCH(BombExplosionEffect_SpawnExplosion, &GlobalNamespace::BombExplosionEffect::SpawnExplosion, void, GlobalNamespace::BombExplosionEffect* self, UnityEngine::Vector3 pos
){

    if(getMainConfig().NoBombs.GetValue() == true)

    {
        self->debrisCount = 0;
        self->explosionParticlesCount = 0;
        BombExplosionEffect_SpawnExplosion(self, pos);
    }
    BombExplosionEffect_SpawnExplosion(self, pos);
};

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
    
    
    auto gameObject = self->musicPackPromoBanner->get_gameObject();
    
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
    QuestUI::Register::RegisterModSettingsViewController<MoreMainUI *>(modInfo, "More By Core");
    QuestUI::Register::RegisterMainMenuModSettingsViewController<MoreMainUI *>(modInfo, "More By Core");
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
    INSTALL_HOOK(getLogger(), NoteCutParticlesEffect_SpawnParticles);
    INSTALL_HOOK(getLogger(), BombExplosionEffect_SpawnExplosion);
    INSTALL_HOOK(getLogger(), PauseMenuManager_ContinueButtonPressed);
    INSTALL_HOOK(getLogger(), PauseMenuManager_ShowMenu);
    getLogger().info("Installed all hooks!");
}