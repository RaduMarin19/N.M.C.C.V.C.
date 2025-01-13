#pragma once

enum class WizardType {
    REMOVE_OPPONENT_CARD,       // Elimina o carte a adversarului de pe tabla care acopera o carte proprie.
    REMOVE_ROW,                 // Elimina un intreg rand de carti de pe tabla (minim 3 pozitii, cu o carte proprie vizibila).
    COVER_OPPONENT_CARD,        // Acopera o carte a oponentului cu o carte proprie de valoare mai mica din mana.
    CREATE_PIT,                 // Transforma un spatiu gol de pe tabla intr-o groapa.
    MOVE_OWN_STACK,             // Muta un teanc de carti cu propria carte deasupra pe o alta pozitie goala pe tabla.
    GAIN_ETER_CARD,             // Capata o extra carte Eter care se plaseaza imediat pe tabla.
    MOVE_OPPONENT_STACK,        // Muta un teanc de carti cu cartea adversarului deasupra pe o alta pozitie goala pe tabla.
    MOVE_EDGE_ROW,              // Muta un rand de la "marginea" tablei pe o alta "margine" (minim 3 pozitii).
};

