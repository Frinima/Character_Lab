# Inventory och utrustning

Första etappen separerar upphämtning, förvaring och utrustning. Inventoryt är ett rutnät med 10 × 6 rutor. Varje föremålsdefinition anger ikon, namn, storlek och eventuell utrustningsplats. Den befintliga chesten använder en stiliserad rustningsikon och tar 2 × 3 rutor.

## Sparade föremålsdefinitioner

Samtliga definitioner är riktiga Data Assets under `/Game/Items/Definitions`. Ikonerna är importerade Texture2D-assets under `/Game/UI/ItemIcons`, med redigerbara SVG-original i `Content/UI/Icons`.

| Data Asset | Föremål | Storlek | Utrustningsbart |
| --- | --- | --- | --- |
| DA_Chest | Chest Armor | 2 × 3 | Ja, chest |
| DA_Gem | Ädelsten | 1 × 1 | Nej |
| DA_Scroll | Skriftrulle | 1 × 2 | Nej |
| DA_Ore | Malmstycke | 2 × 2 | Nej |
| DA_Sword | Svärd | 1 × 3 | Vapen, efter engångskonfigureringen nedan |

Chesten laddas från en konfigurerad assetreferens i `Config/DefaultGame.ini`. Ingen tillfällig definition skapas vid spelstart. Saknad referens rapporteras som ett fel, utan hårdkodad reservdefinition. Asset Manager är konfigurerad att inkludera definitionerna och deras referenser vid paketering.

Tryck K i ett Development-spel för att lägga till ett exemplar av de fyra testföremålen. Tryck flera gånger för att testa fullt inventory. Föremål som inte får plats läggs inte till. Detta är ett testverktyg, inte permanent startutrustning; inga nya föremål placeras automatiskt i banan. Testföremålen kan flyttas, släppas på marken och plockas upp med det vanliga flödet.

Öppna en definition i Content Browser för att ändra namn, storlek, ikon eller utrustningsdata. Spara och starta om Play efter ändringen; en ändring av storlek mitt under en pågående session omplacerar inte befintliga instanser. Dataändringar kräver normalt inget C++-bygge. Listan för K och chestreferensen finns under ARPGPlayerController i DefaultGame.ini.

`Tools/CreateItemAssets.py` är ett engångsverktyg för att skapa saknade assets och verifiera referenser. Det körs i Unreals Python-miljö och skriver inte över befintliga assets eller sparar någon bana. `Tools/RenderItemIcons.cjs` konverterar SVG-originalen till PNG för import. Spelet använder de sparade assetsen, inte dessa skript.

## Spelarflöde

- I öppnar och stänger inventoryt. Det öppnas även efter upphämtning eller när ett fel behöver visas.
- Klick på lootskylten behåller tidigare förflyttning till inom tre meter. Föremålet hamnar därefter i inventoryt, inte på kroppen. Knapp 1 plockar upp närmaste tillgängliga föremål inom räckhåll.
- Klicka på ikonen och sedan Utrusta valt föremål. Föremålet flyttas till utrustningsplatsen och visas på kroppen. Dess tidigare rutor blir lediga.
- Välj den utrustade ikonen och Ta av till inventory. Samma föremål placeras i första lediga område. Finns inte tillräckligt med sammanhängande plats förblir det utrustat.
- Flytta ett förvarat föremål genom att välja ikonen och klicka i en tom ruta. Den klickade rutan blir föremålets övre vänstra hörn. Överlappning och placering utanför rutnätet avvisas.

Knapp 1 använder inte längre Blueprintens gamla direktutrustning eller markdrop. Utrustning och avtagning sker i inventoryt.

### Drag and drop

- Dra ikonen mellan rutorna. Den ruta du grep i hålls under pekaren; hela föremålet måste få plats.
- Dra chesten till Chest eller svärdet till Vapen. Båda kan vara utrustade samtidigt. Fel sorts föremål avvisas, och en upptagen plats kräver att den befintliga utrustningen tas av först. Dra tillbaka till lediga rutor för att ta av föremålet.
- Dra utanför inventorypanelen och släpp över spelvyn för att lägga föremålet på marken bredvid karaktären, inte vid en avlägsen musposition. Detta fungerar även direkt från utrustningsplatsen.
- Ett markdrop visas med en klickbar namnskylt. Upphämtning använder samma avståndsregel som tidigare och bevarar föremålets ID.
- Ogiltig placering, fullt inventory, avbrutet drag eller släpp utanför spelfönstret lämnar föremålet kvar. Släpp över annan UI, exempelvis HUD, är inte en markdrop-yta.

UMG DragDropOperation används för draget. Modellen ändras först när släppet validerats. Vid markdrop skapas och förbereds en dold ARPGDroppedItem innan föremålet tas bort ur inventoryt; misslyckad förberedelse tar endast bort den nya tomma drop-aktören. Därefter visas namnskylten. DragCancelled flyttar aldrig föremål.

## Ansvar och data

`ARPGItemDefinition` är en Primary Data Asset för gemensam föremålsdata, enligt Unreals [standardmodell för Data Assets](https://dev.epicgames.com/documentation/en-us/unreal-engine/data-assets-in-unreal-engine). En definition kan återanvändas av flera individuella föremål.

`ARPGInventoryComponent` på PlayerState äger individuella ID:n, rutnätspositioner och utrustningstillstånd. Det kontrollerar placering, ledig plats och utrustningsplats innan tillstånd ändras. Föremål med EquipmentSlot None kan lagras men inte utrustas. Komponenten innehåller ingen UMG-kod.

`ARPGInventoryWidget` visar modellens tillstånd och begär operationer. `ARPGEquipmentPresentation` observerar samma tillstånd och visar eller döljer utrustningsmeshen. Upphämtning döljer världsföremålet först efter lyckad insättning i inventoryt. Det gamla Blueprint-flödet anropas inte längre av knapp 1 eller lootskylten.

## Bygg och speltest

Användaren bygger. Spara och stäng Unreal Editor, generera om projektfiler vid behov och bygg Character_LabEditor med Development Editor och Win64. Öppna sedan ARPG-banan.

### Engångskonfigurering av svärdet efter bygge

Efter första bygget med vapenstödet och omstart av editorn, kör följande i Output Logs vanliga Cmd-fält, utanför Play:

```text
py "E:/Projekt/Unreal/Arpg/Character_Lab/Tools/ConfigureSword.py"
```

Skriptet sparar DA_Sword med EquipmentSlot Weapon, WeaponMesh SM_Sword_Prototype, AttachmentSocket HandGrip_R och identitetstransform. Det ändrar ingen bana. Kontrollera meddelandet `SWORD_SETUP_COMPLETE`. `SWORD_SETUP_PENDING` betyder att editorn fortfarande använder den äldre C++-versionen. Skriptet körs inte automatiskt vid spelstart.

Modellen och materialen ligger under `/Game/Weapons`. Socketen HandGrip_R finns redan på karaktärens rigg. Vapnet använder en StaticMeshComponent fäst vid denna socket, utan kollision eller påverkan på navigation. Placering och rotation kan justeras via AttachmentTransform i Data Asset; själva karaktärsriggen ändras inte. Ingen stridsanimation eller ändrad fingerpose ingår i detta steg.

### Test av vapenplatsen

1. Starta Play och tryck K. Dra svärdets ikon till Vapen; rutorna ska frigöras och modellen synas i höger hand.
2. Utrusta chesten samtidigt. Gå runt och kontrollera att svärdet följer handen och att chesten är kvar.
3. Prova svärd på Chest, chest på Vapen och ett andra svärd på en upptagen vapenplats. Föremålen ska ligga kvar och ett fel visas.
4. Dra svärdet tillbaka till tre lediga rutor. Handen ska bli tom. Testa även knappen för utrustning och avtagning.
5. Dra ett utrustat svärd ut i spelvyn. Handen ska bli tom, en namnskylt visas och chesten ska inte påverkas. Plocka upp svärdet och utrusta igen; ingen duplicering får ske.

Automationstestet `ARPG.Inventory.WeaponEquipment` täcker separata platser, ogiltiga vapenreferenser, upptagen plats, avtagning och återupphämtning. Användaren kör det efter bygge.

### Regressionstest av inventoryt

1. Tryck I. Ett tomt rutnät ska visas till höger. Klick på panel, rutor och knappar ska inte förflytta karaktären.
2. Stäng inventoryt. Klicka på chestens skylt från längre än tre meter. Karaktären ska gå fram, skylten försvinna och inventoryt öppnas med en ikon som upptar 2 × 3 rutor. Chesten ska inte synas på kroppen ännu.
3. Välj ikonen och utrusta. Chesten ska synas på kroppen, ikonen ska finnas under Chest och rutorna i väskan bli lediga.
4. Välj den utrustade ikonen och ta av. Chesten ska försvinna från kroppen och återkomma i rutnätet utan att dupliceras.
5. Flytta ikonen till en tom ruta. Prova sedan placering för nära högerkanten eller nederkanten: flytten ska avvisas och den gamla positionen bevaras.
6. Upprepa utrusta och ta av flera gånger. Antalet föremål ska vara oförändrat. Stäng och öppna inventoryt; innehållet ska finnas kvar under samma spelsession.
7. Starta om Play och testa upphämtning med 1 inom tre meter. Föremålet ska även här hamna i inventoryt, aldrig direkt på kroppen.
8. Kontrollera förflyttning samt GAS med H, Q och J. Testa också inventoryt i den fönsterstorlek du normalt spelar med.
9. Dra chesten mellan rutor och prova överlappning samt släpp nära rutnätets kant. En ogiltig flytt ska lämna den kvar. Prova att greppa ikonen i olika rutor.
10. Dra till utrustningsfältet och sedan tillbaka till rutnätet. Kontrollera både ikonen och karaktärens mesh. Knappbaserad utrustning ska fortfarande fungera.
11. Dra ut i spelvyn och släpp. Kontrollera att endast en skylt visas bredvid spelaren och att ikonen försvinner. Plocka upp och upprepa, även direkt från utrustningsplatsen.
12. Prova Escape under draget, släpp utanför spelfönstret och släpp på panelens text eller HUD. Inget föremål får försvinna eller hamna på marken. Kontrollera att vanliga världsklick fungerar efter avslutat eller avbrutet drag.
13. Tryck K och kontrollera fyra olika ikoner med storlekarna ovan. Dra dem bredvid varandra, testa upptagna rutor och försök utrusta ett icke utrustningsbart föremål. Upprepa K tills vissa föremål inte får plats.
14. Släpp till exempel ädelstenen och svärdet på marken. Kontrollera namnskyltar, upphämtning och att rätt ikon och storlek kommer tillbaka. Kontrollera även att chesten fortfarande går att utrusta med sin sparade meshreferens.

Automationstestet `ARPG.Inventory.GridAndEquipment` använder föremål i storlekarna 1 × 1, 1 × 2 och 2 × 3. Det kontrollerar placering, fullt inventory, överlappning, ogiltiga storlekar, utrustning, avtagning, upptagen utrustningsplats och unika instans-ID:n. Testet körs av användaren efter bygge via Session Frontend. Ytterligare spelbara testföremål läggs till med K.

## Avgränsningar och temporära delar

- Inventoryt är för lokalt spel. Det är inte sparat till disk eller replikerat för multiplayer. Innehållet återställs vid en ny Play-session.
- Rotation, stapling, automatiskt utrustningsbyte och fler utrustningsplatser återstår. Utrustningen påverkar ännu inte GAS-egenskaper.
- TEMP: K ger en konfigurerad testomgång i Development. Ta bort testkommandot när ordinarie lootkällor täcker testbehoven. Föremålsdefinitionerna är inte längre tillfälliga C++-objekt.
- TEMP: kopplingen till banans enda chest använder befintliga aktörstaggar. Kopplingen till karaktärens visuella chest-komponent använder Blueprint-fältet ChestArmorMesh. Ersätt med placerade pickup-aktörer med explicita definitioner och konfigurerade komponentreferenser när baninnehåll och karaktär migreras. Det generella inventoryt beror inte på dessa namn.
- TEMP: ikonerna är stiliserad prototypgrafik, inte renderingar av slutliga föremålsmodeller. Ersätt grafiken via definitionernas Icon-fält när den slutliga visuella stilen är bestämd.
- TEMP: SM_Sword_Prototype är enkel testgrafik med redigerbar OBJ-källa i Content/Weapons/Source. Ersätt WeaponMesh i DA_Sword med slutlig svärdsmodell och anpassa grepp/animation när vapenutseendet bestäms. Modellens pivot ligger mitt i greppet; klingan pekar längs lokal +Z.
- TEMP: panelen byggs med UMG i C++, har prototypmått och är utformad för standardrutnätet. Flytta layout och stil till en Widget Blueprint vid UI-förfining. I och 1 använder nuvarande direkta tangentbindningar; flytta dessa till Enhanced Input tillsammans med övrig styrning.
- TEMP: markdrop använder initiala placeringsvärden (150 cm åt sidan och åtta sökriktningar) samt en enkel native UMG-skylt. Gör placering och skyltstil innehållsstyrda vid nästa loot-förfining.

De fem Data Assets och deras Texture2D-referenser har skapats med Unreals editorverktyg och tidigare inventoryflöde har testats av användaren. Svärdets testmodell och material är importerade; HandGrip_R har lästs från karaktärens faktiska rigg. Den nya vapenkoden och automationstestet är inte byggda eller körda av agenten. DA_Sword behöver engångskonfigureras efter användarens bygge, innan speltestet ovan.
