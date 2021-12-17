/* Language definition: English.
   (The extra parens are to detect accidental extra commas.) */
const struct Language ENGLISH = {
    // Message_Credits_InsufficentTechToReceive
    ("(-p%0I)<<< Space Dock Message >>>\n"
     "\n"
     "FROM: %0P\n"
     "  Starbase %0d\n"
     "\n"
     "We do not have sufficient tech to\n"
     "receive credit transfers.\n"
     "\n"
     "Required total: %1d\n"),

    // Message_Credits_InsufficentTechToSend
    ("(-p%0I)<<< Space Dock Message >>>\n"
     "\n"
     "FROM: %0P\n"
     "  Starbase %0d\n"
     "\n"
     "We do not have sufficient tech to\n"
     "send credit transfers.\n"
     "\n"
     "Required total: %1d\n"),

    // Message_Credits_Transferred
    ("(-z%0I)<<< Space Dock Message >>>\n"
     "\n"
     "FROM: %0P\n"
     "  Starbase %0d\n"
     "\n"
     "We have transferred %2d mc\n"
     "to the starbase at planet %1d,\n"
     "%1P\n"),

    // Message_MinefieldLaid_Prefix
    ("(-l%1I)<<< Space Dock Message >>>\n"
     "\n"
     "FROM: %0P\n"
     "  Starbase %0d\n"
     "\n"),

    // Message_MinefieldLaid_Web
    ("We have converted our\n"
     "torpedoes into web mines\n"),

    // Message_MinefieldLaid_Normal
    ("We have converted our\n"
     "torpedoes into deep space mines\n"),

    // Message_MinefieldLaid_Suffix
    ("and laid them in a field centered\n"
     "at ( %2d , %3d )\n"
     " %4d mines were laid\n"
     "Mine field ID# %1d now contains\n"
     " %5d mine units and is\n"
     " %6d light years in radius\n"),

    // Message_MinefieldSwept_Web
    ("(-p%0I)<<< Starbase Message >>>\n"
     "\n"
     "From: %0P\n"
     "  Starbase %0d\n\n"
     "Detected enemy mine field #%1d\n"
     "at ( %2d, %3d )\n"
     "They are %4A WEB mines!\n"
     "Minefield is %5d ly across.\n"),

    // Message_MinefieldSwept_Normal
    ("(-p%0I)<<< Starbase Message >>>\n"
     "\n"
     "From: %0P\n"
     "  Starbase %0d\n\n"
     "Detected enemy mine field #%1d\n"
     "at ( %2d, %3d )\n"
     "They are %4A mines!\n"
     "Minefield is %5d ly across.\n"),

    // Message_MinefieldSwept_Fighters
    ("Using fighters to sweep mines.\n"
     "%6d mines destroyed.\n"
     "%7d units remain.\n"),

    // Message_MinefieldSwept_Beams
    ("Using beam weapons to sweep mines.\n"
     "%6d mines destroyed.\n"
     "%7d units remain.\n"),

    // Message_MinefieldScooped_Web
    ("(-p%0I)<<< Starbase Message >>>\n"
     "\n"
     "From: %0P\n"
     "  Starbase %0d\n\n"
     "Detected our WEB mine field #%1d\n"
     "at ( %2d, %3d )\n"
     "Minefield is %4d ly across.\n"),

    // Message_MinefieldScooped_Normal
    ("(-p%0I)<<< Starbase Message >>>\n"
     "\n"
     "From: %0P\n"
     "  Starbase %0d\n\n"
     "Detected our mine field #%1d\n"
     "at ( %2d, %3d )\n"
     "Minefield is %4d ly across.\n"),

    // Message_MinefieldScooped_Action
    ("Gathering mines.\n"
     "We made %5d torpedoes.\n"),

    // Message_Transport_LoadNotPermitted
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We were ordered to load starship parts.\n"
     "Unfortunately, our ship is not able\n"
     "to carry starship parts.\n"),

    // Message_Transport_LoadNoParts
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We were ordered to load starship parts.\n"
     "Unfortunately, the starbase #%1d\n"
     "at %1P\n"
     "did not have any matching parts\n"
     "in storage.\n"),

    // Message_Transport_LoadConflictingParts
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We were ordered to load starship parts.\n"
     "Unfortunately, we already parts of\n"
     "a different type aboard and cannot\n"
     "load another type.\n"),

    // Message_Transport_LoadNoSpace
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We were ordered to load starship parts.\n"
     "Unfortunately, we didn't have any space\n"
     "in our cargo room to take the parts.\n"),

    // Message_Transport_LoadSuccess
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We have successfully loaded %2d parts\n"
     "from starbase #%1d\n"
     "at %1P\n"),

    // Message_Transport_UnloadNoParts
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We were ordered to unload starship\n"
     "parts, but did not have anything\n"
     "to unload."),

    // Message_Transport_UnloadSuccess
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We have unloaded %2d parts\n"
     "to starbase #%1d\n"
     "at %1P"),

    // Message_Transport_TrimmedComponents
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We are overloaded!\n"
     "We had to jettison %1d parts\n"
     "weighing %2d kt total!\n"),

    // Message_Transport_TrimmedCargo
    ("(-s%0I)<<< Fleet Message >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We are overloaded!\n"
     "We had to jettison %1d kt cargo!\n"),

    // SendConfig_Header
    ("(-h0000)<<< Starbase Plus Reloaded >>>\n\n"
     "Configuration:\n"),

    // SendConfig_Continuation
    ("(-h0000)<<< Starbase Plus Reloaded >>>\n\n"
     "Configuration (continued):\n"),

    // ReportShip_Header
    ("(-f%0I)<<< Special Transport >>>\n"
     "\n"
     "FROM: %0S\n"
     "  Ship %0d\n"
     "\n"
     "We are currently carrying components\n"
     "with a total weight of %1d kt:\n"),

    // ReportShip_Continuation
    ("(-f%0I)<<< Special Transport >>>\n"
     "\n"
     "(continued inventory)\n"),

    // Continuation
    ("(continued on next page)\n"),
};
