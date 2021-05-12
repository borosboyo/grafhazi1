# Elso grafika hazi 2021 tavasz / Trying out OpenGL 3, GLSL

**"Plágiumnak minősül mások szellemi termékének forrásmegjelölés nélküli felhasználása, függetlenül attól, hogy szóban, írásban, Interneten vagy bámely más csatornán jutott el a házifeladat beadójához, amely szabály alól csak az előadásfóliák, a tantárgy oktatóinak szóbeli tanácsai, és a grafházi doktor levelei képeznek kivételt. Plágium esetén a feladatra adható pontokat negatív előjellel számoljuk el, és ezzel párhuzamosan a tett súlyosságának megfelelő fórumon eljárást indítunk."**

Gráf megjelenítő fókusszal: Készítsen programot, amely egy véletlen gráfot esztétikusan megjelenít és lehetőséget ad a felhasználónak annak tetszőleges részének kinagyítására, mialatt a maradék rész még mindig látszik. A gráf 50 csomópontból áll, telítettsége 20%-os (a lehetséges élek 20% valódi él). Az esztétikus elrendezés érdekében a csomópontok helyét egyrészt heurisztikával, másrészt a hiperbolikus sík szabályainak megfelelő erő-vezérelt gráfrajzoló algoritmussal kell meghatározni. A fókuszálás érdekében a gráfot a hiperbolikus síkon kell elrendezni és a Beltrami-Klein módszerrel a képernyőre vetíteni. A fókuszálás úgy történik, hogy a gráfot a hiperbolikus síkon eltoljuk úgy, hogy az érdekes rész a hiperboloid aljára kerüljön. Az egyes csomópontok a hiperbolikus sík körei, amelyek a csomópontot azonosító textúrával bírnak.

![](/logo.png)

