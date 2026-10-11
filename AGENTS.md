# Projektriktlinjer

- Användaren kompilerar/bygger när det krävs. Agenten beskriver vad som behöver byggas och testas, men startar inte byggen.
- Använd alltid Unreals standardlösningar när sådana finns. Undantag kräver synnerligen starka skäl som presenteras för användaren före implementation.
- Målet är ett komplett, spelbart ARPG med ambitionen att vara minst lika bra som Path of Exile 1. Funktionsomfattningen definieras löpande; målbilden är inte en beställning att implementera allt på en gång.
- Åtgärda grundorsaken, aldrig bara symptomen. Workarounds får endast användas tillfälligt för felsökning och får inte finnas kvar i den färdiga produkten.
- Den färdiga produkten ska vara så modulär och datadriven som möjligt. Separera speldata, spellogik och presentation.
- Hårdkodning är tillåten i tidiga experiment men ska snarast ersättas med riktiga, modulära och datadrivna lösningar.
- Markera alla temporära workarounds och hårdkodningar med `TEMP` i koden eller i ett separat dokument. Ange varför de finns och vad som krävs för att ersätta eller ta bort dem.
- Redovisa vad som faktiskt har verifierats och vad användaren behöver testköra. Bevara befintligt fungerande beteende och användarens ändringar.
