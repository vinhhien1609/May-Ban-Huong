
#ifndef VDM_LANGUAGE_H
#define VDM_LANGUAGE_H

/**
 * \defgroup        vdm_language Vending machine language settings
 * \brief           Language settings
 * \{
 */

#include <stdint.h>

/* The supported language */
typedef enum
{
    VDM_LANG_VNI = 0x00,   /* Vietnamese */
    VDM_LANG_ENGLISH = 0x01,   /* English */
    VDM_LANG_MAX    /* The end of languages */
} vdm_language_t;


/* The IDs of strings */
typedef enum
{
    VDM_LANG_YES_NO = 0,
    VDM_LANG_CONFIRM_RESET,
    /**/
		VDM_LANG_PASSWORD,
		VDM_LANG_CURRNT_CONFIG,
		VDM_LANG_TOTAL_MOUNT,
		VDM_LANG_TOTAL_SETTING,
		VDM_LANG_ERROR_INFO,
		VDM_LANG_EN_CONTROL_HUMIDITY,
		VDM_LANG_HUMUDITY_SETTING,
		VDM_LANG_ACCEPT_MOUNT,
		VDM_LANG_TIME_RUN_INSENSE,
		VDM_LANG_TOTAL_INSENSE_CELL,
		VDM_LANG_INSENSE_CELL_MORE,
		VDM_LANG_DELETE_ERROR,
		VDM_LANG_MODE,
    VDM_LANG_ID_RESTORE_FACTORY_FIRMWARE,
    VDM_LANG_ID_MAX
} vdm_language_id_t;

/**
 * @brief           Load current language settings from storage.
 */
void vdm_language_restore_from_storage(void);

/**
 * @brief           Set new language settings
 */
void vdm_language_set_language(vdm_language_t language);

/**
 * @brief           Get current language settings
 * @retval          Language id @ref vdm_language_id_t
 */
vdm_language_t vdm_language_get_current_lang();


/**
 * @brief           Get text form language id
 * @retval          Text
 */
const char* vdm_language_get_text(vdm_language_id_t language_id);

/**
 * \}
 */

#endif /* VDM_LANGUAGE_H */
