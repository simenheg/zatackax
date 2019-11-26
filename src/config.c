#include "config.h"

unsigned int scorecap = DEFAULT_SCORECAP;
bool fullscreen = DEFAULT_FULLSCREEN;
bool sound = DEFAULT_SOUND;
bool music = DEFAULT_MUSIC;
bool weapons = DEFAULT_WEAPONS;
bool holes = DEFAULT_HOLES;
bool broadcasts = DEFAULT_BROADCASTS;
bool border = DEFAULT_BORDER;
bool duelmode = DEFAULT_DUELMODE;
const char *settingNames[] = {"fullscreen", "sound", "music", "weapons",
                              "holes", "broadcasts", "border", "duelmode"};
bool *settings[8] = {&fullscreen, &sound, &music, &weapons, &holes,
                     &broadcasts, &border, &duelmode};

/**
 * Return that path of the directory where the config file resides.
 */
char *getConfigPath(void)
{
    char *home = getenv("HOME");
    char *xdg_config_home = getenv("XDG_CONFIG_HOME");

    int strlen = snprintf(NULL, 0, "%s/%szatackax",
                          xdg_config_home ? xdg_config_home : home,
                          xdg_config_home ? "" : ".config/");
    char *config_path = calloc(strlen + 1, sizeof(char));
    snprintf(config_path, strlen + 1, "%s/%szatackax",
             xdg_config_home ? xdg_config_home : home,
             xdg_config_home ? "" : ".config/");

    return config_path;
}

/**
 * Ensure that the directory where the config file resides exists.
 *
 * Return zero on success, or -1 if an error occurred (in which case,
 * errno is set appropriately).
 */
int ensureConfigPath(void)
{
    char *configPath = getConfigPath();
    int status = mkdir(configPath, S_IRWXU);
    free(configPath);
    return status;
}

/**
 * Return the full config file name.
 */
char *getConfigFile(void)
{
    char *configPath = getConfigPath();

    int strlen = snprintf(NULL, 0, "%s/settings.cfg", configPath);
    char *configFile = calloc(strlen + 1, sizeof(char));
    snprintf(configFile, strlen + 1, "%s/settings.cfg", configPath);

    free(configPath);

    return configFile;
}

/**
 * Save the current settings to the config file.
 */
void saveSettings(void)
{
    ensureConfigPath();
    char *filename = getConfigFile();
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        if (olvl >= O_NORMAL) {
            fprintf(stderr, "Couldn't open file '%s' for writing.\n",
                    filename);
        }
        free(filename);
        return;
    }

    for (unsigned int i = 0; i < sizeof(settings) / sizeof(bool*); ++i) {
        fprintf(file, "%s = %d\n", settingNames[i], *settings[i]);
    }

    fprintf(file, "scorecap = %d\n", scorecap);
    for (unsigned int i = 0; i < MAX_PLAYERS; ++i) {
        fprintf(file, "%dn = %s\n", i + 1, (&players[i])->name);
        fprintf(file, "%dc = %d\n", i + 1, (&players[i])->color);
        fprintf(file, "%da = %d\n", i + 1, (&players[i])->ai);
        fprintf(file, "%dl = %d\n", i + 1, (&players[i])->lkey);
        fprintf(file, "%dw = %d\n", i + 1, (&players[i])->wkey);
        fprintf(file, "%dr = %d\n", i + 1, (&players[i])->rkey);
    }

    if (olvl >= O_NORMAL && ferror(file)) {
        fprintf(stderr, "Error writing to file '%s'.\n", filename);
    }

    free(filename);
    fclose(file);
}

/**
 * Restore previous settings from the config file.
 */
void restoreSettings(void)
{
    char *filename = getConfigFile();
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        if (olvl >= O_DEBUG) {
            fprintf(stderr, "Couldn't restore settings from file %s.\n",
                    filename);
        }
        free(filename);
        return;
    }

    char setting[STRBUF];
    char value[STRBUF];
    int line = 0;
    bool valid = false;

    while ((fscanf(file, "%s = %s\n", setting, value)) != EOF) {
        valid = false;
        for (unsigned int i = 0; i < sizeof(settings)/sizeof(bool*); ++i) {
            if (strncmp(settingNames[i], setting, STRBUF) == 0) {
                /* We have a matched setting */
                *settings[i] = atoi(value);
                valid = true;
                break;
            }
        }
        if (!valid && strncmp("scorecap", setting, STRBUF) == 0) {
            scorecap = atoi(value);
            valid = true;
        }
        else if (!valid && isdigit(setting[0])) {
            struct player *p = &players[setting[0] - '0' - 1];
            switch (setting[1]) {
            case 'n': /* Name */
                strncpy(p->name, value, PLAYER_NAME_LEN);
                valid = true;
                break;
            case 'c': /* Color */
                p->color = atoi(value);
                valid = true;
                break;
            case 'a': /* AI */
                p->ai = atoi(value);
                valid = true;
                break;
            case 'l': /* Left key */
                p->lkey = atoi(value);
                valid = true;
                break;
            case 'w': /* Weapon key */
                p->wkey = atoi(value);
                valid = true;
                break;
            case 'r': /* Right key */
                p->rkey = atoi(value);
                valid = true;
                break;
            default:
                break;
            }
        }
        if (!valid && olvl >= O_NORMAL) {
            fprintf(stderr, "Unknown config format in '%s', line "
                    "%d: %s\n", filename, line, setting);
        }
        ++line;
    }

    free(filename);
    fclose(file);
}
