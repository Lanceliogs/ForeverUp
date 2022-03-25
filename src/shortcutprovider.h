#ifndef SHORTCUTPROVIDER_H
#define SHORTCUTPROVIDER_H

#include <windows.h>

class ShortcutProvider
{
public:
    /*
    -------------------------------------------------------------------
    Description:
    Creates the actual 'lnk' file (COM should be initialized and released inside the method).

    Parameters:
    pszTargetfile    - File name of the link's target.
    pszTargetargs    - Command line arguments passed to link's target.
    pszLinkfile      - File name of the actual link file being created.
    pszDescription   - Description of the linked item.
    iShowmode        - ShowWindow() constant for the link's target.
    pszCurdir        - Working directory of the active link.
    pszIconfile      - File name of the icon file used for the link.
    iIconindex       - Index of the icon in the icon file.

    Returns:
    HRESULT value >= 0 for success, < 0 for failure.
    --------------------------------------------------------------------
    */
    static HRESULT create(LPSTR pszTargetfile, LPSTR pszTargetargs,
        LPSTR pszLinkfile, LPSTR pszDescription,
        int iShowmode, LPSTR pszCurdir,
        LPSTR pszIconfile, int iIconindex);
};

#endif // SHORTCUTPROVIDER_H
