#pragma once

enum class ElementalType {
    CONTROLLED_EXPLOSION, // Explozie controlata: Tabla explodeaza! Obs: Conditii speciale in functie de regula jocului.
    DESTROY,              // Distrugere: Elimina din joc ultima carte jucata de adversar.
    FIRE,                 // Flacari: Intoarce iluzia adversarului cu fata in sus si joaca o carte pe oricare pozitie de pe tabla.    !!DONE!!
    LAVA,                 // Lava: Toate cartile vizibile cu un numar specific se intorc in mainile proprietarilor.                   !!DONE!!
    ASHES,                // Din cenusa: Alege o carte proprie eliminata din joc si joac-o imediat.                                   !!DONE!!
    SPARKS,               // Scantei: Alege orice carte proprie acoperita de adversar si joac-o pe alta pozitie.
    WHIRLWIND,            // Vifor: Intoarce o carte vizibila a oponentului in mana sa.                                               !!DONE!!
    SQUALL,               // Vijelie: Toate cartile acoperite de alte carti se intorc in mainile proprietarilor.                      !!DONE!!
    HURRICANE,            // Uragan: Shift-eaza un rand complet ocupat cu 1 pozitie in directia dorita.
    FLURRY,               // Rafala: Muta o carte vizibila pe o pozitie adiacenta cu o carte cu un numar mai mic.                      !!DONE!!
    MIRAGE,               // Miraj: Inlocuieste propria iluzie plasata cu o alta iluzie. Obs: Disponibil doar daca se joaca cu iluzii. !!DONE!!
    STORM,                // Furtuna: Elimina din joc orice teanc de carti ce contine 2 sau mai multe carti.                           !!DONE!!
    TIDE,                 // Maree: Interschimba pozitiile a 2 teancuri de carti.                                                      !!DONE!!
    MIST,                 // Ceata: Joaca inca o iluzie. Obs: Un jucator nu poate avea 2 iluzii simultan pe tabla.                     !!DONE!! 
    WAVE,                 // Val: Muta un teanc pe o pozitie adiacenta goala si joaca o carte pe noua pozitie.                         !!DONE!!
    WHIRLPOOL,            // Vartej de apa: Muta 2 carti de pe acelasi rand pe un spatiu gol. Cartea mai mare se pune deasupra.        !!DONE!! + TODO
    TSUNAMI,              // Tsunami: Blocheaza un rand pentru urmatoarea tura a adversarului.                                         !!DONE!!
    WATERFALL,            // Cascada: Teancurile de pe un rand "cad" spre o margine, formand un teanc.
    SUPPORT,              // Sprijin: Valoarea unei carti proprii 1/2/3 creste cu 1.                                                   !!DONE!!
    EARTHQUAKE,           // Cutremur: Elimina de pe tabla toate cartile vizibile cu numarul 1.                                        !!DONE!!
    SHATTER,              // Sfarama: Valoarea unei carti a adversarului 2/3/4 scade cu 1.                                             !!DONE!!
    BORDERS,              // Granite: Plaseaza o carte neutra pentru a defini o granita a tablei.                                      !!DONE!!
    AVALANCHE,            // Avalansa: Shift-eaza doua teancuri adiacente pe orizontala/verticala cu 1 pozitie.
    BOULDER,               // Bolovan: Acopera orice iluzie cu o carte, fara a intoarce iluzia.                                         !!DONE!!
    NO_ELEMENT //!DONE :)
};
