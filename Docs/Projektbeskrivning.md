# Övergripande projektbeskrivning för ARPG

## Målbild och omfattning

Målet är ett komplett, spelbart actionrollspel med ambitionen att vara minst lika bra som Path of Exile 1. Spelaren utforskar världen, besegrar fiender, samlar föremål och utvecklar sin karaktär genom utrustning, förmågor och progression.

Funktionsomfattningen definieras löpande. Målbilden anger riktning och ambitionsnivå, inte att samtliga funktioner ska byggas samtidigt eller att spelet ska kopiera POE1. Vi utvecklar små sammanhängande, spelbara delar och förfinar respektive område när det blir aktuellt.

## Spelets huvudområden

### Spelgrund och styrning

Kamera, förflyttning, målval och interaktion med världen. Utgångspunkten är ett top down perspektiv med click to move. Styrningen ska kunna utvecklas utan att övriga spelsystem behöver byggas om.

### Karaktärer och strid

Spelare och fienders egenskaper, resurser, förmågor, skada, försvar och tillstånd. Gameplay Ability System, GAS, ska introduceras som grund för relevanta delar av detta område. Inventory, uppdrag och gränssnitt har egna ansvarsområden och ska inte samlas i GAS.

### Föremål och utrustning

Föremålsdefinitioner, individuella föremål, loot, upphämtning, inventory, utrustning och användbara föremål. Upphämtning och utrustning är separata handlingar. Inventory ska kunna representera föremål med ikoner och olika utrymmesbehov.

### Progression och karaktärsbyggen

Erfarenhet, nivåer, specialisering och val som låter spelaren utveckla olika spelstilar. Förmågor, utrustning och progression ska samverka och ge meningsfulla val över tid.

### Fiender och stridsmöten

Fiendetyper, beteenden, bossar och regler för var och hur fiender uppträder. Möten ska kunna varieras och anpassas till världens innehåll och önskad svårighetsgrad.

### Värld och utforskning

Områden, banor, interaktiva objekt, förflyttning mellan platser och innehåll att upptäcka. Behovet av handbyggda respektive genererade miljöer beslutas senare.

### NPC och uppdrag

Dialoger, questgivare, villkor, mål, belöningar och uppdragens tillstånd. Uppdrag ska kunna bero på tidigare handlingar och världens tillstånd. Innehållsdefinitionerna hålls åtskilda från varje spelares framsteg.

### Spelloop och ekonomi

Sambandet mellan strid, belöningar och fortsatt utveckling. Området omfattar balans, föremålens värde, möjliga system för handel och crafting samt långsiktig variation och endgame. Exakt omfattning är ännu öppen.

### Gränssnitt och presentation

HUD, menyer, inventoryvyer, verktygstips och återkoppling genom animation, ljud och visuella effekter. Presentationen ska göra spelets tillstånd och spelarens val tydliga utan att äga spellogiken. Läsbarhet och tillgänglighet beaktas från början.

### Sparande och spelsessioner

Karaktärer, inventory, progression, uppdragsstatus och relevant världstillstånd som behöver bestå mellan speltillfällen. Här ingår också hur spel startas och återupptas. Singleplayer, eventuell multiplayer och onlinetjänster är öppna beslut med konsekvenser för grundarkitekturen.

### Innehållsverktyg och arbetsflöden

Verktyg som gör det enkelt att skapa, koppla samman, balansera och kontrollera spelinnehåll utan specialkod för varje fall. Exempel är redigering av uppdrag, föremål, lootregler, fiender och stridsmöten samt verktyg för att inspektera och testa innehållet.

Ett questverktyg ska exempelvis kunna uttrycka: NPC xxx erbjuder uppdrag A endast när uppdrag yyy är slutfört. Samma definition kan ange mål och belöningar. Spelets uppdragssystem utvärderar villkoren mot spelarens framsteg; verktyget används för att skapa definitionerna.

Begreppet questgenerator innebär här i första hand ett verktyg för att definiera och kombinera innehåll, inte ett beslut om automatiskt eller AI-genererade uppdrag. Validering bör upptäcka exempelvis saknade referenser och motsägelsefulla beroenden. Unreals befintliga redigeringsmöjligheter används först; egna verktyg byggs när ett konkret behov motiverar dem.

### Teknisk grund och kvalitet

Modularitet, datahantering, versionshantering, felsökning, tester, prestanda och paketering. Systemens ansvar och beroenden ska vara tydliga. Tillfälliga lösningar ska vara spårbara och ersättas innan de blir permanent grund.

## Gemensamma principer

- Följ [projektriktlinjerna i AGENTS.md](../AGENTS.md), inklusive prioriteringen av Unreals standardlösningar och att användaren ansvarar för byggen.
- Separera speldata, spellogik och presentation. HUD visar exempelvis hälsa men bestämmer den inte.
- Utforma system för återanvändning och datadrivet innehåll utan att bygga generell infrastruktur för ännu okända behov.
- Verifiera små hela kedjor, exempelvis besegra fiende → få loot → plocka upp → utrusta → påverka nästa strid.
- Ansvarsområdena ovan är en systemkarta, inte ett beslut om motsvarande antal kodmoduler.

## Första nästa etapp

Introducera en rudimentär HUD och GAS med en liten verifierbar spelkedja: verkliga karaktärsegenskaper för hälsa och en förmågeresurs, en enkel testförmåga med resurskostnad och återhämtningstid samt möjlighet att ta skada och återställa hälsa. HUD visar det faktiska speltillståndet.

## Fortsatt förfining

Varje område kan senare få en egen beskrivning av ansvar, beroenden, första spelbara omfattning, data, testkriterier och öppna beslut. Tidigt behöver vi klargöra framför allt multiplayerambition och vad den första kompletta spelbara versionen ska innehålla. Ordning, omfattning och detaljlösningar revideras efter erfarenheterna från speltester.
