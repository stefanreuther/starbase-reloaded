/* Language definition: German.
   (The extra parens are to detect accidental extra commas.)
   (Do not use umlauts for charset independence.) */
const struct Language GERMAN = {
    // Message_Credits_InsufficentTechToReceive
    ("(-p%0I)<<< Raumdockbericht >>>\n"
     "\n"
     "VON: %0P\n"
     "  Basis %0d\n"
     "\n"
     "Wir haben nicht ausreichend Technologie,\n"
     "um Ueberweisungen zu empfangen.\n"
     "\n"
     "Minimum erforderlich: %1d\n"),

    // Message_Credits_InsufficentTechToSend
    ("(-p%0I)<<< Raumdockbericht >>>\n"
     "\n"
     "VON: %0P\n"
     "  Basis %0d\n"
     "\n"
     "Wir haben nicht ausreichend Technologie,\n"
     "um Geld zu ueberweisen.\n"
     "\n"
     "Minimum erforderlich: %1d\n"),

    // Message_Credits_Transferred
    ("(-z%0I)<<< Raumdockbericht >>>\n"
     "\n"
     "VON: %0P\n"
     "  Basis %0d\n"
     "\n"
     "Wir haben %2d mc ueberwiesen\n"
     "an die Sternenbasis bei Planet %1d,\n"
     "%1P\n"),

    // Message_MinefieldLaid_Prefix
    ("(-l%1I)<<< Raumdockbericht >>>\n"
     "\n"
     "VON: %0P\n"
     "  Basis %0d\n"
     "\n"),

    // Message_MinefieldLaid_Web
    ("Wir haben unsere Torpedos als\n"
     "Fangminen gelegt.\n"),

    // Message_MinefieldLaid_Normal
    ("Wir haben unsere Torpedos als\n"
     "normale Minen gelegt.\n"),

    // Message_MinefieldLaid_Suffix
    ("Das Minenfeld befindet sich\n"
     "an (%2d, %3d).\n"
     "%4d Minen wurden gelegt.\n"
     "Minenfeld ID #%1d enthaelt nun\n"
     "%5d Minen und hat einen\n"
     "Radius von %6d Lichtjahren.\n"),

    // Message_MinefieldSwept_Web
    ("(-p%0I)<<< Raumdockbericht >>>\n"
     "\n"
     "Von: %0P\n"
     "  Basis %0d\n\n"
     "Gegnerisches Fangminenfeld #%1d\n"
     "gefunden an (%2d, %3d).\n"
     "Das Feld besteht aus %4A Fangminen.\n"
     "Es hat einen Durchmesser von %5d ly.\n"),

    // Message_MinefieldSwept_Normal
    ("(-p%0I)<<< Raumdockbericht >>>\n"
     "\n"
     "Von: %0P\n"
     "  Basis %0d\n\n"
     "Gegnerisches Minenfeld #%1d\n"
     "gefunden an (%2d, %3d).\n"
     "Das Feld besteht aus %4A Minen.\n"
     "Es hat einen Durchmesser von %5d ly.\n"),

    // Message_MinefieldSwept_Fighters
    ("Nutze Jaeger zum Raeumen der Minen.\n"
     "%6d Minen geraeumt.\n"
     "%7d Minen verbleiben.\n"),

    // Message_MinefieldSwept_Beams
    ("Nutze Geschuetze zum Raeumen der Minen.\n"
     "%6d Minen geraeumt.\n"
     "%7d Minen verbleiben.\n"),

    // Message_MinefieldScooped_Web
    ("(-p%0I)<<< Raumdockbericht >>>\n"
     "\n"
     "Von: %0P\n"
     "  Basis %0d\n\n"
     "Eigenes Fangminenfeld #%1d\n"
     "gefunden an (%2d, %3d).\n"
     "Es hat einen Durchmesser von %4d ly.\n"),

    // Message_MinefieldScooped_Normal
    ("(-p%0I)<<< Raumdockbericht >>>\n"
     "\n"
     "Von: %0P\n"
     "  Basis %0d\n\n"
     "Eigenes Minenfeld #%1d\n"
     "gefunden an (%2d, %3d).\n"
     "Es hat einen Durchmesser von %4d ly.\n"),

    // Message_MinefieldScooped_Action
    ("Sammle Minen ein.\n"
     "Wir haben %5d Torpedos gesammelt.\n"),

    // Message_Transport_LoadNotPermitted
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir wurden beauftragt, Raumschiffteile\n"
     "zu laden.\n"
     "Leider ist unser Schiff nicht in der\n"
     "Lage, Raumschiffteile zu tragen.\n"),

    // Message_Transport_LoadNoParts
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir wurden beauftragt, Raumschiffteile\n"
     "zu laden.\n"
     "Leider hatte die Sternenbasis #%1d\n"
     "an %1P\n"
     "keine passenden Teile auf Lager.\n"),

    // Message_Transport_LoadConflictingParts
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir wurden beauftragt, Raumschiffteile\n"
     "zu laden.\n"
     "Leider haben wir bereits Teile eines\n"
     "anderen Typs an Bord und koennen keine\n"
     "weiteren Teile laden.\n"),

    // Message_Transport_LoadNoSpace
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir wurden beauftragt, Raumschiffteile\n"
     "zu laden.\n"
     "Leider haben wir keinen Platz im\n"
     "Frachtraum, um die Teile aufzunehmen.\n"),

    // Message_Transport_LoadSuccess
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir haben erfolgreich %2d Teile\n"
     "geladen von Sternenbasis #%1d\n"
     "bei %1P\n"),

    // Message_Transport_UnloadNoParts
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir wurden beauftragt, Raumschiffteile\n"
     "zu entladen, hatten aber keine an Bord.\n"),

    // Message_Transport_UnloadSuccess
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir haben %2d Raumschiffteile\n"
     "entladen an Sternenbasis #%1d\n"
     "bei %1P"),

    // Message_Transport_TrimmedComponents
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir sind ueberladen!\n"
     "Wir mussten %1d Raumschiffteile\n"
     "mit insgesamt %2d kt Gewicht\n"
     "ueber Bord werfen!\n"),

    // Message_Transport_TrimmedCargo
    ("(-s%0I)<<< Flottenbericht >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir sind ueberladen!\n"
     "Wir mussten %1d kt Fracht\n"
     "ueber Bord werfen!\n"),

    // SendConfig_Header
    ("(-h0000)<<< Starbase Plus Reloaded >>>\n\n"
     "Konfiguration:\n"),

    // SendConfig_Continuation
    ("(-h0000)<<< Starbase Plus Reloaded >>>\n\n"
     "Konfiguration (Fortsetzung):\n"),

    // ReportShip_Header
    ("(-f%0I)<<< Spezial-Transport >>>\n"
     "\n"
     "VON: %0S\n"
     "  Schiff %0d\n"
     "\n"
     "Wir transportieren Raumschiffteile\n"
     "von insgesamt %1d kt Masse:\n"),

    // ReportShip_Continuation
    ("(-f%0I)<<< Spezial-Transport >>>\n"
     "\n"
     "(Inventar, Fortsetzung)\n"),

    // SendConfig_Footer
    ("(weiter auf der naechsten Seite)\n"),
};
