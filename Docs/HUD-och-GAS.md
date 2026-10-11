# Första etappen med HUD och GAS

Den här etappen kopplar en enkel UMG HUD till GAS med hälsa, mana och en läkningsförmåga. Befintlig förflyttning, loot och utrustning behålls. Implementation finns i källkoden men behöver byggas och speltestas av användaren.

## Ansvarsfördelning

- `ARPGPlayerState` äger Ability System Component och Attribute Set. Karaktären är systemets avatar. Grundvärden och förmåga tilldelas en gång per PlayerState, inte varje gång HUD skapas.
- `ARPGAttributeSet` innehåller hälsa, maxhälsa, mana och maxmana samt värdegränser och replikeringsnotifieringar.
- `ARPGHealAbility` använder vanliga Gameplay Effects för läkning, kostnad och cooldown. Aktivering avvisas vid full eller noll hälsa, för lite mana eller aktiv cooldown.
- `ARPGHUDWidget` visar GAS-värden och anropar förmågeaktivering. Egenskapsändringar uppdaterar HUD via delegater. En presentationstimer läser återstående cooldown från GAS.
- `ARPGPlayerController` kopplar input, lokal HUD och avatar till PlayerState. HUD har en utbytbar widgetklass.

Placeringen av ASC på PlayerState följer ett [standardupplägg i Unreal](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-component-and-gameplay-attributes-in-unreal-engine). Multiplayer, sparande och karaktärsdöd är inte färdiga system i denna etapp. Den nuvarande Blueprint-karaktären implementerar ännu inte Ability System Interface; åtkomst sker via PlayerState. Innan generella fiendeattacker och målbaserade effekter införs behöver även avatarens GAS-gränssnitt definieras.

## Bygg och testa

1. Spara och stäng Unreal Editor. Nya reflekterade C++-klasser och ett plugin har tillkommit; använd ett vanligt fullständigt bygge, inte enbart Live Coding.
2. Generera om projektfiler från `Character_Lab.uproject` om Visual Studio inte visar de nya filerna. Bygg editor-targeten `Character_LabEditor` med konfigurationen Development Editor och Win64.
3. Öppna projektet och spela `/Game/ARPG/Lvl_ARPG`. HUD ska visas nedtill med hälsa 100/100 och mana 100/100.
4. Tryck H en gång: hälsa ska bli 75. Tryck Q eller klicka på läkningsknappen: hälsa ska bli 100, mana 80 och cooldown visas i tre sekunder.
5. Tryck H igen och försök läka under cooldown: varken hälsa eller mana ska ändras av det avvisade försöket. Efter cooldown ska läkning fungera igen.
6. Upprepa skada och läkning tills mana är slut. Ytterligare läkning ska avvisas utan kostnad. Q vid full hälsa ska inte heller kosta mana.
7. Tryck J för att återställa grundvärdena. Det tar inte bort en aktiv cooldown. Upprepa H tills hälsan når noll och kontrollera att den inte blir negativ. J återställer även från noll; död och respawn är ännu inte implementerade.
8. Klicka på HUD och dess knapp: det ska inte skicka karaktären mot marken bakom panelen. Testa därefter click to move, klickupphämtning från långt håll och knapp 1 för att ta av chesten.
9. Stoppa och starta Play igen: en enda HUD ska skapas och värdena ska börja om. Prova även en annan fönsterstorlek.

H och J är tillfälliga testkontroller för ett lokalt spel och är avstängda i Shipping-byggen. Det finns ännu ingen automatisk manaregenerering.

Automationstestet `ARPG.GAS.VitalBounds` kan köras via Unreals Session Frontend efter bygge. Det kontrollerar grundläggande gränser samt cooldown-effektens tagg och varaktighetstyp, inte hela spelkedjan.

## Temporära delar som ska ersättas

- `ARPGPrototypeEffects`: hårdkodade startvärden 100/100, läkning 30, kostnad 20 mana, cooldown 3 sekunder och testskada 25. Ersätt balansvärdena med redigerbara Gameplay Effect assets innan fler riktiga förmågor byggs; ta bort testskadan när ordinarie strid finns.
- `ARPGPlayerState` och `ARPGHealAbility`: native prototypklasser som standardval. Klassreferenserna är redigerbara och ska peka på innehållsdefinitioner när dessa skapas.
- `ARPGHUDWidget` och HUD-positioneringen i `ARPGPlayerController`: native testlayout, färger, mått och text för en enda läkningsförmåga. Ersätt med en designad Widget Blueprint och förmågornas presentationsdata när layout och förmågefält förfinas.
- Input för Q, H och J: direkta testbindningar. Flytta spelkommandon till redigerbara Enhanced Input actions när styrningssystemet förfinas. Ta bort felsökningskommandona från produktionsflödet.
- Tidigare prototypkod: `ChestLootWidget` och `ARPGPlayerController` använder en specifik chest, aktörstaggar, Blueprint-fältnamn, fasta kameravärden och direkta tangentbindningar. `ARPGGameMode` väljer karaktär med en fast asset-sökväg. Dessa befintliga beroenden är inte ombyggda i denna etapp; ersätt dem med explicita föremålsdefinitioner, referenser och konfigurerbar styrning i respektive nästa systemsteg.

## Verifieringsstatus

Källkoden och använda API-signaturer har granskats mot den lokala Unreal 5.8-installationen. Inget bygge, automationstest eller visuellt speltest har körts av agenten. Byggbarhet och beteende behöver därför verifieras enligt checklistan ovan.
