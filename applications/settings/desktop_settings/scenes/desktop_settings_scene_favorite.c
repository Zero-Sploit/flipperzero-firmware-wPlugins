#include "../desktop_settings_app.h"
#include "../ext/services/applications.h"
#include "desktop_settings_scene.h"
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <fap_loader/fap_loader_app.h>

static bool favorite_fap_selector_item_callback(
    FuriString* file_path,
    void* context,
    uint8_t** icon_ptr,
    FuriString* item_name) {
    UNUSED(context);
#ifdef APP_FAP_LOADER
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool success = fap_loader_load_name_and_icon(file_path, storage, icon_ptr, item_name);
    furi_record_close(RECORD_STORAGE);
#else
    UNUSED(file_path);
    UNUSED(icon_ptr);
    UNUSED(item_name);
    bool success = false;
#endif
    return success;
}

static bool favorite_fap_selector_file_exists(char* file_path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool exists = storage_file_exists(storage, file_path);
    furi_record_close(RECORD_STORAGE);
    return exists;
}

static void desktop_settings_scene_favorite_submenu_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void desktop_settings_scene_favorite_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    Submenu* submenu = app->submenu;
    submenu_reset(submenu);

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);
    uint32_t pre_select_item = 0;

    for(size_t i = 0; i < FLIPPER_APPS_COUNT2; i++) {
        submenu_add_item(
            submenu,
            FLIPPER_APPS2[i].name,
            i,
            desktop_settings_scene_favorite_submenu_callback,
            app);

        if(primary_favorite == 0) { // Select favorite item in submenu
            if((app->settings.favorite_primary.is_external &&
                !strcmp(FLIPPER_APPS2[i].name, FAP_LOADER_APP_NAME)) ||
               (!strcmp(FLIPPER_APPS2[i].name, app->settings.favorite_primary.name_or_path))) {
                pre_select_item = i;
            }
        } else if(primary_favorite == 1) {
            if((app->settings.favorite_secondary.is_external &&
                !strcmp(FLIPPER_APPS2[i].name, FAP_LOADER_APP_NAME)) ||
               (!strcmp(FLIPPER_APPS2[i].name, app->settings.favorite_secondary.name_or_path))) {
                pre_select_item = i;
            }
        } else if(primary_favorite == 2) {
            if((app->settings.favorite_tertiary.is_external &&
                !strcmp(FLIPPER_APPS2[i].name, FAP_LOADER_APP_NAME)) ||
               (!strcmp(FLIPPER_APPS2[i].name, app->settings.favorite_tertiary.name_or_path))) {
                pre_select_item = i;
            }
        } else if(primary_favorite == 3) {
            if((app->settings.favorite_quaternary.is_external &&
                !strcmp(FLIPPER_APPS2[i].name, FAP_LOADER_APP_NAME)) ||
               (!strcmp(FLIPPER_APPS2[i].name, app->settings.favorite_quaternary.name_or_path))) {
                pre_select_item = i;
            }
        }
    }
    submenu_add_item(
        submenu,
        "None (disable)",
        FLIPPER_APPS_COUNT2 + 1,
        desktop_settings_scene_favorite_submenu_callback,
        app);

    if(primary_favorite == 0) {
        submenu_set_header(submenu, "Primary favorite app:");
    } else if(primary_favorite == 1) {
        submenu_set_header(submenu, "Secondary favorite app:");
    } else if(primary_favorite == 2) {
        submenu_set_header(submenu, "Tertiary favorite app:");
    } else if(primary_favorite == 3) {
        submenu_set_header(submenu, "Quaternary favorite app:");
    }
    submenu_set_selected_item(submenu, pre_select_item); // If set during loop, visual glitch.

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewMenu);
}

bool desktop_settings_scene_favorite_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;
    FuriString* temp_path = furi_string_alloc_set_str(EXT_PATH("apps"));

    uint32_t primary_favorite =
        scene_manager_get_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event >= (FLIPPER_APPS_COUNT2 + 1)) {
            if(primary_favorite == 0) {
                app->settings.favorite_primary.is_external = false;
                strncpy(app->settings.favorite_primary.name_or_path, "", MAX_APP_LENGTH);
            } else if(primary_favorite == 1) {
                app->settings.favorite_secondary.is_external = false;
                strncpy(app->settings.favorite_secondary.name_or_path, "", MAX_APP_LENGTH);
            } else if(primary_favorite == 2) {
                app->settings.favorite_tertiary.is_external = false;
                strncpy(app->settings.favorite_tertiary.name_or_path, "", MAX_APP_LENGTH);
            } else if(primary_favorite == 3) {
                app->settings.favorite_quaternary.is_external = false;
                strncpy(app->settings.favorite_quaternary.name_or_path, "", MAX_APP_LENGTH);
            }

            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
            furi_string_free(temp_path);
            return consumed;
        }
        if(strcmp(FLIPPER_APPS2[event.event].name, FAP_LOADER_APP_NAME) != 0) {
            if(primary_favorite == 0) {
                app->settings.favorite_primary.is_external = false;
                strncpy(
                    app->settings.favorite_primary.name_or_path,
                    FLIPPER_APPS2[event.event].name,
                    MAX_APP_LENGTH);
            } else if(primary_favorite == 1) {
                app->settings.favorite_secondary.is_external = false;
                strncpy(
                    app->settings.favorite_secondary.name_or_path,
                    FLIPPER_APPS2[event.event].name,
                    MAX_APP_LENGTH);
            } else if(primary_favorite == 2) {
                app->settings.favorite_tertiary.is_external = false;
                strncpy(
                    app->settings.favorite_tertiary.name_or_path,
                    FLIPPER_APPS2[event.event].name,
                    MAX_APP_LENGTH);
            } else if(primary_favorite == 3) {
                app->settings.favorite_quaternary.is_external = false;
                strncpy(
                    app->settings.favorite_quaternary.name_or_path,
                    FLIPPER_APPS2[event.event].name,
                    MAX_APP_LENGTH);
            }
        } else {
            const DialogsFileBrowserOptions browser_options = {
                .extension = ".fap",
                .icon = &I_unknown_10px,
                .skip_assets = true,
                .hide_ext = true,
                .item_loader_callback = favorite_fap_selector_item_callback,
                .item_loader_context = app,
                .base_path = EXT_PATH("apps"),
            };

            if(primary_favorite == 0) { // Select favorite fap in file browser
                if(favorite_fap_selector_file_exists(
                       app->settings.favorite_primary.name_or_path)) {
                    furi_string_set_str(temp_path, app->settings.favorite_primary.name_or_path);
                }
            } else if(primary_favorite == 1) {
                if(favorite_fap_selector_file_exists(
                       app->settings.favorite_secondary.name_or_path)) {
                    furi_string_set_str(temp_path, app->settings.favorite_secondary.name_or_path);
                }
            } else if(primary_favorite == 2) {
                if(favorite_fap_selector_file_exists(
                       app->settings.favorite_tertiary.name_or_path)) {
                    furi_string_set_str(temp_path, app->settings.favorite_tertiary.name_or_path);
                }
            } else if(primary_favorite == 3) {
                if(favorite_fap_selector_file_exists(
                       app->settings.favorite_quaternary.name_or_path)) {
                    furi_string_set_str(temp_path, app->settings.favorite_quaternary.name_or_path);
                }
            }

            submenu_reset(app->submenu);
            if(dialog_file_browser_show(app->dialogs, temp_path, temp_path, &browser_options)) {
                if(primary_favorite == 0) {
                    app->settings.favorite_primary.is_external = true;
                    strncpy(
                        app->settings.favorite_primary.name_or_path,
                        furi_string_get_cstr(temp_path),
                        MAX_APP_LENGTH);
                } else if(primary_favorite == 1) {
                    app->settings.favorite_secondary.is_external = true;
                    strncpy(
                        app->settings.favorite_secondary.name_or_path,
                        furi_string_get_cstr(temp_path),
                        MAX_APP_LENGTH);
                } else if(primary_favorite == 2) {
                    app->settings.favorite_tertiary.is_external = true;
                    strncpy(
                        app->settings.favorite_tertiary.name_or_path,
                        furi_string_get_cstr(temp_path),
                        MAX_APP_LENGTH);
                } else if(primary_favorite == 3) {
                    app->settings.favorite_quaternary.is_external = true;
                    strncpy(
                        app->settings.favorite_quaternary.name_or_path,
                        furi_string_get_cstr(temp_path),
                        MAX_APP_LENGTH);
                }
            }
        }
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }

    furi_string_free(temp_path);
    return consumed;
}

void desktop_settings_scene_favorite_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    DESKTOP_SETTINGS_SAVE(&app->settings);
    submenu_reset(app->submenu);
}
