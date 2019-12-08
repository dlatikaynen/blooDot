DECLARE SUB MONSTER (x!, y!, z!, a!)
DECLARE SUB DELAY (dauer%)
DECLARE SUB EXPLOSION (xp!, yp!, rad!, col!)
DECLARE SUB kern (x!, y!)
DECLARE SUB NEPOMUK ()
DECLARE SUB TIMERS (level%, ZUSATZ!)
DECLARE SUB SCHLEUSE (x!, y!, z!)
DECLARE SUB TRESOR (x!, y!, z!)
DECLARE SUB EIS (x!, y!)
DECLARE SUB WASSER (a!, B!)
DECLARE SUB TOR (x!, y!, f!)
DECLARE SUB MAUER (x!, y!)
DECLARE SUB FELS (x!, y!)
DECLARE SUB DING (art!, x!, y!, f!)
DECLARE SUB LEVELS (level%)
DECLARE SUB BLACK (a!, B!, z!)
DECLARE SUB TISCH (x!, y!, z!)
CLS
SCREEN 12
CLEAR , , 3000
COLOR 15

mauergrosse! = 4 + INT(((PMAP(30, 0) - PMAP(0, 0) + 1) * (1) + 7) / 8) * 4 * (PMAP(30, 1) - PMAP(0, 1) + 1)
skerngrosse! = 4 + INT(((PMAP(29, 0) - PMAP(1, 0) + 1) * (1) + 7) / 8) * 4 * (PMAP(29, 1) - PMAP(1, 1) + 1)

DIM SHARED level  AS INTEGER
DIM SHARED lives AS INTEGER
DIM SHARED score AS INTEGER
DIM SHARED scorem AS INTEGER
DIM SHARED feld(60, 42) AS INTEGER
DIM SHARED r AS INTEGER
DIM SHARED x
DIM SHARED y
DIM SHARED schlussel(20) AS INTEGER
DIM SHARED wwasser(21) AS SINGLE
DIM SHARED wwassery(21) AS SINGLE
DIM SHARED wmauer(20) AS SINGLE
DIM SHARED s1
DIM SHARED s2
DIM SHARED nepomuk1(mauergrosse) AS SINGLE
DIM SHARED nepomuk2(mauergrosse) AS SINGLE
DIM SHARED nepomuk3(mauergrosse) AS SINGLE
DIM SHARED nepomuk4(mauergrosse) AS SINGLE
DIM SHARED skern(skerngrosse) AS SINGLE
DIM SHARED schusse
DIM SHARED xpo
DIM SHARED ypo
DIM SHARED rpo
DIM SHARED zettel(20)
DIM SHARED viehx(30)
DIM SHARED viehy(30)
DIM SHARED viehr(30)

NEPOMUK

x = 0
y = 0
r = 0
s1 = 0
s2 = 0

level = 1
lives = 3
score = 0
scorem = 0
schusse = 200

GOSUB beleg
GOTO weiter1
beleg:
FOR beleg% = 3 TO 60 STEP 3
FOR beleg1% = 3 TO 42 STEP 3
feld(beleg%, beleg1%) = 0
NEXT beleg1%
NEXT beleg%
FOR beleg% = 1 TO 20
schlussel(beleg%) = 0
wwasser(beleg%) = 0
wmauer(beleg%) = 0
NEXT beleg%
l5t = 0: s1 = 0: s2 = 0
RETURN
weiter1:
COLOR 15

'-------------------------------

INPUT "Bitte das Level eingeben > ", level

'-------------------------------

erstes:

IF level > 25 THEN
CLS
SCREEN 13
FOR i = 1 TO 23
IF i < 15 THEN COLOR i ELSE COLOR i + 20
LOCATE i, i
PRINT "Lade 2. Teil..."
NEXT i
CLOSE
RESET
KILL "ubergang.dat"
OPEN "ubergang.dat" FOR OUTPUT AS #1
PRINT #1, level, lives, score, scorem, schusse
CLOSE #1
CHAIN "nepomuk2.bas"
END IF
CLS
SCREEN 12
FOR lx = 0 TO 600 STEP 30
MAUER lx, 0: feld(lx / 10, 0) = 1
MAUER lx, 420: feld(lx / 10, 42) = 1
NEXT lx
FOR ly = 30 TO 390 STEP 30
MAUER 0, ly: feld(0, ly / 10) = 1
MAUER 600, ly: feld(60, ly / 10) = 1
NEXT ly
LEVELS level
GOSUB kopfzeile
GOSUB kp2
IF level = 3 THEN
ON TIMER(1) GOSUB maintimer
OPEN "timer3.da1" FOR INPUT AS #1
OPEN "timer3.da2" FOR INPUT AS #2
TIMER ON
END IF
IF level = 7 OR level = 8 OR level = 9 OR level = 12 OR level = 14 THEN
ON TIMER(1) GOSUB maintimer
TIMER ON
END IF
IF level = 15 THEN
OPEN "timer15.da1" FOR INPUT AS #1
ON TIMER(1) GOSUB maintimer
TIMER ON
END IF
IF level = 16 THEN
OPEN "timer16.da1" FOR INPUT AS #1
OPEN "timer16.da2" FOR INPUT AS #2
OPEN "timer16.da3" FOR INPUT AS #3
OPEN "timer16.da4" FOR INPUT AS #4
ON TIMER(1) GOSUB maintimer
TIMER ON
END IF
IF level = 17 THEN
OPEN "timer17.da1" FOR INPUT AS #1
ON TIMER(1) GOSUB maintimer
TIMER ON
END IF
IF level = 18 OR level = 19 OR level = 20 OR level = 21 OR level = 22 OR level = 23 OR level = 24 OR level = 25 THEN
ON TIMER(1) GOSUB maintimer
TIMER ON
END IF
IF r = 1 THEN PUT (x, y), nepomuk1
IF r = 2 THEN PUT (x, y), nepomuk2
IF r = 3 THEN PUT (x, y), nepomuk3
IF r = 4 THEN PUT (x, y), nepomuk4

taste:
    kybd$ = INKEY$
     SELECT CASE kybd$
          CASE CHR$(0) + "H": GOTO oben
          CASE CHR$(0) + "P": GOTO unten
          CASE CHR$(0) + "K": GOTO links
          CASE CHR$(0) + "M": GOTO rechts
          CASE "S": GOTO levelende
          CASE "s": GOTO levelende
          CASE "p": GOTO nehmen
          CASE "P": GOTO nehmen
          CASE " ": GOTO schiessen
          CASE CHR$(27): TIMER OFF
               CLS
               SCREEN 12
               COLOR 2
               PRINT "Auf Wiedersehen !"
               INPUT "Qbasic oder DOS (Q / D) :", a$
               COLOR 3
               PRINT "Danke !"
               IF UCASE$(a$) = "Q" THEN COLOR 15: SYSTEM
               IF UCASE$(a$) = "D" THEN SLEEP 1: CLS : SCREEN 12: COLOR 3: PRINT "ZurÅck ins QBasic mit 'EXIT' !": SHELL
               SYSTEM
          CASE ELSE
          END SELECT
        GOTO taste

links:
GOSUB loschen
IF r <> 1 THEN r = 1 ELSE x = x - 30: r = 1
GOSUB abfrage
GOTO taste

rechts:
GOSUB loschen
IF r <> 2 THEN r = 2 ELSE x = x + 30: r = 2
GOSUB abfrage
GOTO taste

unten:
GOSUB loschen
IF r <> 3 THEN r = 3 ELSE y = y + 30: r = 3
GOSUB abfrage
GOTO taste

oben:
GOSUB loschen
IF r <> 4 THEN r = 4 ELSE y = y - 30: r = 4
GOSUB abfrage
GOTO taste

abfrage:
IF level = 3 OR level = 25 THEN GOSUB spezialabfrage
IF (feld(x / 10, y / 10)) = 0 THEN GOTO leeresfeld
IF (feld(x / 10, y / 10)) = 1 THEN GOSUB gegenmauer: GOTO leeresfeld
IF (feld(x / 10, y / 10)) <> 0 AND (feld(x / 10, y / 10)) <> 1 THEN GOSUB sonstiges

leeresfeld:

IF r = 1 THEN PUT (x, y), nepomuk1
IF r = 2 THEN PUT (x, y), nepomuk2
IF r = 3 THEN PUT (x, y), nepomuk3
IF r = 4 THEN PUT (x, y), nepomuk4

RETURN

loschen:

IF r = 1 THEN PUT (x, y), nepomuk1
IF r = 2 THEN PUT (x, y), nepomuk2
IF r = 3 THEN PUT (x, y), nepomuk3
IF r = 4 THEN PUT (x, y), nepomuk4

RETURN

gegenmauer:

IF r = 1 THEN x = x + 30
IF r = 2 THEN x = x - 30
IF r = 3 THEN y = y - 30
IF r = 4 THEN y = y + 30

RETURN

sonstiges:

ON (feld(x / 10, y / 10)) GOSUB g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12, g13, g14, g15, g16, g17, g18, g19, g20, g21, g22, g23, g24, g25, g26, g27, g28, g29, g30, g31, g32, g33, g34
GOSUB genommen

RETURN

g1:
art = 0
RETURN
g2:
feld(x / 10, y / 10) = 0
art = 1
RETURN
g3:
feld(x / 10, y / 10) = 0
art = 2
RETURN
g4:
feld(x / 10, y / 10) = 0
art = 3
RETURN
g5:
feld(x / 10, y / 10) = 0
art = 4
RETURN
g6:
feld(x / 10, y / 10) = 0
art = 5
RETURN
g7:
feld(x / 10, y / 10) = 0
art = 6
RETURN
g8:
feld(x / 10, y / 10) = 0
art = 7
GOSUB trank
RETURN
g9:
feld(x / 10, y / 10) = 0
art = 8
RETURN
g10:
feld(x / 10, y / 10) = 0
art = 9
RETURN
g11:
SELECT CASE level
CASE 14
FOR a = 0 TO 600 STEP 30
MAUER a, 0
MAUER a, 420
NEXT
FOR a = 0 TO 420 STEP 30
MAUER 0, a
MAUER 600, a
NEXT
FOR a = 30 TO 570 STEP 30
FOR B = 30 TO 390 STEP 30
trankl14a = feld(a / 10, B / 10)
SELECT CASE trankl14a
CASE 1
MAUER a, B
CASE 2 TO 18
DING trankl14a - 1, a, B, 1
CASE 19
EIS a, B
CASE 21
TOR a, B, 1
CASE 25
FELS a, B
CASE ELSE
END SELECT
NEXT
NEXT
IF schlussel(1) = 0 THEN DING 14, 60, 60, 4
IF schlussel(2) = 0 THEN DING 14, 540, 60, 2
IF schlussel(3) = 0 THEN DING 14, 60, 360, 14
IF schlussel(4) = 0 THEN DING 14, 540, 360, 1
TOR 300, 150, 14
TOR 210, 210, 1
TOR 390, 210, 2
TOR 300, 240, 4
IF feld(24, 6) <> 0 THEN DING 7, 240, 60, 4
IF feld(30, 6) <> 0 THEN DING 7, 300, 60, 2
IF feld(36, 6) <> 0 THEN DING 7, 360, 60, 14
IF feld(42, 6) <> 0 THEN DING 7, 420, 60, 1
FOR i = 1 TO 4
IF feld(viehx(i) / 10, viehy(i) / 10) <> 0 THEN MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
END SELECT
feld(x / 10, y / 10) = 0
art = 10
RETURN
g12:
feld(x / 10, y / 10) = 0
art = 11
RETURN
g13:
feld(x / 10, y / 10) = 0
art = 12
RETURN
g14:
feld(x / 10, y / 10) = 0
art = 13
IF level = 13 THEN zettel(1) = 1
RETURN
g15:
feld(x / 10, y / 10) = 0
IF level = 1 OR level = 2 OR level = 4 OR level = 5 OR level = 9 OR level = 10 OR level = 11 OR level = 12 OR level = 13 OR level = 16 OR level = 17 OR level = 18 OR level = 19 OR level = 21 OR level = 22 OR level = 23 OR level = 25 THEN GOSUB standardschlussel ELSE GOSUB spezialschlussel
art = 14
RETURN
g16:
feld(x / 10, y / 10) = 0
art = 15
RETURN
g17:
feld(x / 10, y / 10) = 0
art = 16
RETURN
g18:
feld(x / 10, y / 10) = 0
schusse = schusse + 2
art = 17
RETURN
g19:
art = 0
SELECT CASE r
CASE 1
eispruf = x
WHILE feld(eispruf / 10, y / 10) = 19
GOSUB sch1
eispruf = eispruf - 30
WEND
x = eispruf
GOSUB abfrage
PUT (x, y), nepomuk1
CASE 2
eispruf = x
WHILE feld(eispruf / 10, y / 10) = 19
GOSUB sch2
eispruf = eispruf + 30
WEND
x = eispruf
GOSUB abfrage
PUT (x, y), nepomuk2
CASE 3
eispruf = y
WHILE feld(x / 10, eispruf / 10) = 19
GOSUB sch3
eispruf = eispruf + 30
WEND
y = eispruf
GOSUB abfrage
PUT (x, y), nepomuk3
CASE 4
eispruf = y
WHILE feld(x / 10, eispruf / 10) = 19
GOSUB sch4
eispruf = eispruf - 30
WEND
y = eispruf
GOSUB abfrage
PUT (x, y), nepomuk4
END SELECT
RETURN
g20:
art = 0
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(x / 10, y / 10) = 0
GOSUB wassertod
RETURN
g21:
IF level = 1 OR level = 2 OR level = 4 OR level = 5 OR level = 9 OR level = 10 OR level = 11 OR level = 12 OR level = 13 OR level = 16 OR level = 17 OR level = 18 OR level = 19 OR level = 21 OR level = 22 OR level = 23 OR level = 25 THEN GOSUB standardtor ELSE GOSUB spezialtor
art = 0
RETURN
g22:
art = 0
GOSUB scha
RETURN
g23:
art = 0
IF level = 13 AND zettel(1) = 1 THEN GOSUB TRESOR ELSE GOSUB gegenmauer
RETURN
g24:
art = 0
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(x / 10, y / 10) = 0
GOSUB wassertod
RETURN
g25:
art = 0
GOSUB gegenmauer
RETURN
g26:
art = 0
GOSUB gegenmauer
RETURN
g27:
art = 0
GOSUB wassertod
RETURN
g28:
art = 0
RETURN
g29:
art = 0
RETURN
g30:
art = 0
RETURN
g31:
art = 0
GOSUB sch2
RETURN
g32:
art = 0
GOSUB sch1
RETURN
g33:
art = 0
GOSUB sch4
RETURN
g34:
art = 0
GOSUB sch3
RETURN

wassertod:
BEEP
RETURN

maintimer:
CALL TIMERS(level%, 0)
RETURN
 
genommen:
ON art GOSUB Mun, sch, her, kre, piz, kuc, tra, rum, EIS, glu, Uhr, reg, zet, slu, sei, kir, pem
RETURN
Mun:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "O3T160L16cba"
scorem = scorem + 1
GOSUB kopfzeile
RETURN
sch:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
score = score + 10
PLAY "T255O4L16abacad"
RETURN
her:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
lives = lives + 1
PLAY "T255O5L32afgecbagda"
GOSUB kopfzeile
RETURN
kre:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "T255O4L1eeddccba               "
RETURN
piz:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
score = score + 20
PLAY "T255O4L32aggfbbddd            "
RETURN
kuc:
LINE (x + 1, y + 1)-(x + 29, y + 30), 0, BF
score = score + 30
PLAY "T255O5L32cfgaf                  "
RETURN
tra:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "T255O4L16cdefg              "
RETURN
rum:
score = score + 50
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "T255O5L64afgegafad              "
RETURN
EIS:
LINE (x + 1, y + 1)-(x + 29, y + 30), 0, BF
score = score + 60
PLAY "T255O4L32cdefefg                                        "
RETURN
glu:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "T255O5L16aab "
RETURN
Uhr:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "T255O4L32ccP16L32aaP16L32gg            "
RETURN
reg:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
schirm = 1
PLAY "T255O4L16deeggd"
RETURN
zet:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
zettl = 1
PLAY "T255O4L16bbdcaaa "
RETURN
slu:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "O4T250L8gef"
RETURN
sei:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "T255O2L32ccccccccccccc"
RETURN
kir:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
PLAY "T255O4L64abcdefgg"
schusse = schusse + 2
GOSUB kp2
RETURN
pem:
LINE (x + 1, y + 1)-(x + 29, y + 29), 0, BF
SOUND 100, 2
SOUND 300, 1
SOUND 200, 3
GOSUB kp2
RETURN

kopfzeile:

SCREEN 12
IF lives > 28 THEN lives = 28
LINE (2, 457)-(123, 472), 5, B
FOR balken = 3 TO lives * 4
LINE (balken, 458)-(balken + 10, 471), 9, BF
NEXT balken
FOR leer = balken + 10 TO 118
LINE (leer, 458)-(leer + 4, 471), 0, BF
NEXT leer
RETURN
kp2: LINE (125, 451)-(629, 479), 0, BF
j = -1
i = 130
schus = schusse
DO
IF schus = 0 THEN EXIT DO
IF schus > 40 THEN LINE (565, 455)-(575, 465), 15: LINE -(565, 475), 15: schus = 40
j = j + 1
IF j = schus THEN EXIT DO
i = i + 10
kern i, 451
LOOP
RETURN

standardtor:
IF schlussel(1) = 1 THEN GOTO levelende
GOSUB gegenmauer
RETURN

standardschlussel:
schlussel(1) = 1
RETURN

spezialtor:
IF level = 3 THEN GOSUB l3spezialtor
IF level = 6 THEN GOSUB l6spezialtor
IF level = 7 THEN GOSUB l7spezialtor
IF level = 8 THEN GOSUB l8spezialtor
IF level = 14 THEN GOSUB l14spezialtor
IF level = 15 THEN GOSUB l15spezialtor
IF level = 20 THEN GOSUB l20spezialtor
IF level = 24 THEN GOSUB l24spezialtor
RETURN

spezialschlussel:
TIMERS level%, 1
RETURN

levelende:

CLOSE
TIMER OFF
PLAY "O3T255L4eefggfedccdee.L8dL2d"
WINDOW (0, 0)-(630, 480)
CLS
SCREEN 0
SCREEN 12
CLS
LINE (0, 0)-(630, 450), 2, B
LINE (5, 5)-(625, 445), 2, B
PAINT (2, 2), 2
COLOR 5
LOCATE 2, 41 - 23 / 2
PRINT "***** LEVEL"; level; "*****"
LOCATE 5, 41 - 17 / 2: COLOR 14
IF scorem < 10 THEN PRINT "Guthaben :    "; scorem
IF scorem > 9 AND scorem < 100 THEN PRINT "Guthaben :   "; scorem
IF scorem > 99 THEN PRINT "MÅnzen :  "; scorem
LOCATE 7, 41 - 13 / 2
IF lives < 10 THEN PRINT "Leben :    "; lives
IF lives > 9 THEN PRINT "Leben :   "; lives
LOCATE 10, 41 - 15 / 2
PRINT "Punkte :"; score
inventarnein:
LOCATE 14, 41 - 37 / 2: COLOR 0
PRINT "Inventarliste anzeigen ? ( J / N )                  "
LOCATE 14, 41 - 37 / 2: COLOR 9
INPUT "Inventarliste anzeigen ? ( J / N ) ", inven$
'IF UCASE$(inven$) = "J" THEN GOSUB inventliste
IF UCASE$(inven$) = "N" THEN GOTO weiter2 ELSE GOTO inventarnein
weiter2:
GOSUB beleg
level = level + 1

GOTO erstes

l3spezialtor:

IF x = 30 AND y = 30 AND schlussel(1) = 1 THEN GOTO levelende
IF x = 30 AND y = 30 AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF x = 120 AND y = 90 AND schlussel(2) = 1 THEN GOSUB l3s2
IF x = 120 AND y = 90 AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF x = 570 AND y = 60 AND schlussel(3) = 1 THEN GOSUB l3s3
IF x = 570 AND y = 60 AND schlussel(3) = 0 THEN GOSUB gegenmauer
IF x = 270 AND y = 360 AND schlussel(4) = 1 THEN GOSUB l3s4
IF x = 270 AND y = 360 AND schlussel(4) = 0 THEN GOSUB gegenmauer
IF x = 330 AND y = 360 AND schlussel(5) = 1 THEN GOSUB l3s5
IF x = 330 AND y = 360 AND schlussel(5) = 0 THEN GOSUB gegenmauer
RETURN
l3s2:
IF r = 1 THEN x = x - 30
IF r = 1 AND feld(9, 9) = 2 THEN
art = 1
GOSUB genommen
feld(9, 9) = 0
END IF
IF r = 2 THEN x = x + 30
RETURN
l3s4:
l3s5:
IF r = 1 THEN x = x - 30
IF r = 2 THEN x = x + 30
RETURN
l3s3:
IF r = 4 THEN y = y - 30
IF r = 3 THEN y = y + 30
RETURN

l6spezialtor:

IF ((x = 240 AND y = 150) OR (x = 330 AND y = 210) OR (x = 270 AND y = 330)) AND schlussel(1) = 1 THEN GOSUB sch4
IF ((x = 240 AND y = 150) OR (x = 330 AND y = 210) OR (x = 270 AND y = 330)) AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF ((x = 300 AND y = 150) OR (x = 240 AND y = 270) OR (x = 330 AND y = 330)) AND schlussel(2) = 1 THEN GOSUB sch4
IF ((x = 300 AND y = 150) OR (x = 240 AND y = 270) OR (x = 330 AND y = 330)) AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF ((x = 360 AND y = 150) OR (x = 300 AND y = 270)) AND schlussel(3) = 1 THEN GOSUB sch4
IF ((x = 360 AND y = 150) OR (x = 300 AND y = 270)) AND schlussel(3) = 0 THEN GOSUB gegenmauer
IF ((x = 270 AND y = 210) OR (x = 360 AND y = 270)) AND schlussel(4) = 1 THEN GOSUB sch4
IF ((x = 270 AND y = 210) OR (x = 360 AND y = 270)) AND schlussel(4) = 0 THEN GOSUB gegenmauer
IF x = 90 AND y = 30 AND schlussel(5) = 1 THEN GOSUB sch2
IF x = 90 AND y = 30 AND schlussel(5) = 0 THEN GOSUB gegenmauer
IF x = 210 AND y = 30 AND schlussel(6) = 1 THEN GOSUB sch2
IF x = 210 AND y = 30 AND schlussel(6) = 0 THEN GOSUB gegenmauer
IF x = 330 AND y = 30 AND schlussel(7) = 1 THEN GOSUB sch2
IF x = 330 AND y = 30 AND schlussel(7) = 0 THEN GOSUB gegenmauer
IF x = 570 AND y = 30 AND schlussel(8) = 1 THEN GOTO levelende
IF x = 570 AND y = 30 AND schlussel(8) = 0 THEN GOSUB gegenmauer
RETURN

l7spezialtor:
IF x = 90 AND y = 360 AND schlussel(1) = 1 THEN GOSUB sch1
IF x = 90 AND y = 360 AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF x = 240 AND y = 330 AND schlussel(2) = 1 THEN GOSUB sch3
IF x = 240 AND y = 330 AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF x = 150 AND y = 390 AND schlussel(3) = 1 THEN GOSUB sch1
IF x = 150 AND y = 390 AND schlussel(3) = 0 THEN GOSUB gegenmauer
IF x = 330 AND y = 120 AND schlussel(4) = 1 THEN GOSUB sch3
IF x = 330 AND y = 120 AND schlussel(4) = 0 THEN GOSUB gegenmauer
IF x = 30 AND y = 390 AND schlussel(5) = 1 THEN GOTO levelende
IF x = 30 AND y = 390 AND schlussel(5) = 0 THEN GOSUB gegenmauer
IF x = 210 AND y = 360 AND schlussel(6) = 1 THEN GOSUB sch1
IF x = 210 AND y = 360 AND schlussel(6) = 0 THEN GOSUB gegenmauer
RETURN

l8spezialtor:
IF x = 30 AND y = 390 AND schlussel(1) = 1 THEN GOTO levelende
IF x = 30 AND y = 390 AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF x = 270 AND y = 30 AND schlussel(3) = 1 THEN BLACK 270, 30, 4: BLACK 240, 30, 4: feld(27, 3) = 0: feld(24, 3) = 0
IF x = 270 AND y = 30 AND schlussel(3) = 0 THEN GOSUB gegenmauer
IF x = 330 AND y = 390 AND schlussel(2) = 1 THEN BLACK 330, 390, 4: BLACK 360, 390, 4: feld(36, 39) = 0: feld(33, 39) = 0
IF x = 330 AND y = 390 AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF x = 570 AND y = 30 AND schlussel(4) = 1 THEN BLACK 570, 30, 4: BLACK 570, 60, 4: feld(57, 3) = 0: feld(57, 6) = 0
IF x = 570 AND y = 30 AND schlussel(4) = 0 THEN GOSUB gegenmauer
RETURN

l14spezialtor:
IF x = 300 AND y = 240 AND schlussel(2) = 1 THEN GOTO levelende
IF x = 300 AND y = 240 AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF x = 180 AND y = 210 AND schlussel(1) = 1 THEN GOSUB sch3
IF x = 180 AND y = 210 AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF x = 390 AND y = 210 AND schlussel(3) = 1 THEN GOSUB sch3
IF x = 390 AND y = 210 AND schlussel(3) = 0 THEN GOSUB gegenmauer
IF x = 300 AND y = 150 AND schlussel(4) = 1 THEN GOSUB sch3
IF x = 300 AND y = 150 AND schlussel(4) = 0 THEN GOSUB gegenmauer
RETURN

l15spezialtor:
IF x = 480 AND y = 180 AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF x = 480 AND y = 180 AND schlussel(1) = 1 THEN GOSUB sch3
IF x = 480 AND y = 240 AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF x = 480 AND y = 240 AND schlussel(2) = 1 THEN GOSUB levelende
RETURN

l20spezialtor:
IF x = 570 AND y = 390 AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF x = 570 AND y = 390 AND schlussel(1) = 1 THEN GOTO levelende
IF x = 570 AND y = 330 AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF x = 570 AND y = 330 AND schlussel(2) = 1 THEN GOSUB scha
IF x = 450 AND y = 30 AND schlussel(3) = 0 THEN GOSUB gegenmauer
IF x = 450 AND y = 30 AND schlussel(3) = 1 THEN GOSUB scha
RETURN

l24spezialtor:
IF x = 540 AND y = 240 AND schlussel(1) = 0 THEN GOSUB gegenmauer
IF x = 540 AND y = 240 AND schlussel(1) = 1 THEN GOSUB scha
IF x = 450 AND y = 30 AND schlussel(2) = 0 THEN GOSUB gegenmauer
IF x = 450 AND y = 30 AND schlussel(2) = 1 THEN GOSUB scha
IF x = 210 AND y = 120 AND schlussel(3) = 0 THEN GOSUB gegenmauer
IF x = 210 AND y = 120 AND schlussel(3) = 1 THEN GOSUB scha
IF x = 30 AND y = 390 AND schlussel(4) = 0 THEN GOSUB gegenmauer
IF x = 30 AND y = 390 AND schlussel(4) = 1 THEN GOTO levelende
RETURN

nehmen:
IF level = 3 AND x = 480 AND y = 330 AND r = 2 THEN BLACK 510, 330, 4: TISCH 510, 300, 2: schlussel(2) = 1: feld(51, 33) = 1
IF level = 3 AND x = 510 AND y = 360 AND r = 4 THEN BLACK 510, 330, 4: TISCH 510, 300, 2: schlussel(2) = 1: feld(51, 33) = 1

GOTO taste

scha:
IF r = 2 THEN x = x + 30
IF r = 1 THEN x = x - 30
IF r = 3 THEN y = y + 30
IF r = 4 THEN y = y - 30
RETURN
sch1:
IF r = 1 THEN x = x - 30
IF r <> 1 THEN GOSUB gegenmauer
RETURN
sch2:
IF r = 2 THEN x = x + 30
IF r <> 2 THEN GOSUB gegenmauer
RETURN
sch3:
IF r = 3 THEN y = y + 30
IF r <> 3 THEN GOSUB gegenmauer
RETURN
sch4:
IF r = 4 THEN y = y - 30
IF r <> 4 THEN GOSUB gegenmauer
RETURN

trank:
SELECT CASE level

CASE 5
l5t = 1
BLACK 60, 150, 2
feld(6, 15) = 0

CASE 12
BLACK 540, 330, 3: BLACK 570, 330, 4
feld(54, 33) = 0: feld(57, 33) = 0

CASE 14
TIMERS 14, 2

CASE 15
BLACK 150, 300, 1

CASE 18
DING 14, 420, 300, 14

CASE 19
BLACK 300, 300, 7

CASE 23
FOR i = 90 TO 180 STEP 30
BLACK 240, i, 4
NEXT i
BLACK 180, 210, 4
BLACK 210, 210, 4
BLACK 270, 180, 4

CASE 24
schusse = schusse * 2
GOSUB kp2

CASE ELSE
END SELECT
  
RETURN

TRESOR:
SELECT CASE level
CASE 13:
feld(3, 36) = 0
BLACK 30, 360, 4
EXPLOSION 30, 360, 9, 14
BLACK 30, 360, 4
x = x + 30
DING 14, 30, 360, 14
feld(3, 36) = 15
CASE ELSE
END SELECT
RETURN

spezialabfrage:
SELECT CASE level

CASE 3:
IF x = s2 AND y = 150 THEN r = 1: x = x - 30
IF x = s1 AND y = 210 THEN r = 2: x = x + 30

CASE 25
IF (x = 150 AND y = 60) OR (x = 450 AND y = 60) OR (x = 360 AND y = 360) OR (x = 240 AND y = 360) OR (x = 270 AND y = 210) OR (x = 480 AND y = 210) THEN GOSUB l25rnd

CASE ELSE
END SELECT
RETURN

l25rnd:
RANDOMIZE TIMER
i = INT(RND * 6) + 1
ON i GOSUB l25a, l25b, l25c, l25d, l25e, l25f
RETURN
l25a:
x = 240
y = 30
r = 4
RETURN
l25b:
x = 360
y = 30
r = 4
RETURN
l25c:
x = 150
y = 210
r = 2
RETURN
l25d:
x = 360
y = 210
r = 2
RETURN
l25e:
x = 150
y = 390
r = 3
RETURN
l25f:
x = 450
y = 390
r = 3
RETURN

schiessen:
IF schusse = 0 THEN GOTO taste
schusse = schusse - 1
ON r GOSUB lsch, rsch, usch, osch
GOSUB nachschuss
GOTO taste

lsch:
FOR lschuss = x - 30 TO 0 STEP -30
IF feld(lschuss / 10, y / 10) <> 0 THEN EXIT FOR
kern lschuss, y
IF lschuss <> x - 30 THEN PUT (lschuss + 31, y + 1), skern, PSET
DELAY 500
NEXT lschuss
xpo = lschuss
ypo = y
rpo = 1
RETURN

rsch:
FOR rschuss = x + 30 TO 600 STEP 30
IF feld(rschuss / 10, y / 10) <> 0 THEN EXIT FOR
kern rschuss, y
IF rschuss <> x + 30 THEN PUT (rschuss - 29, y + 1), skern, PSET
DELAY 500
NEXT rschuss
xpo = rschuss
ypo = y
rpo = 2
RETURN

usch:
FOR uschuss = y + 30 TO 420 STEP 30
IF feld(x / 10, uschuss / 10) <> 0 THEN EXIT FOR
kern x, uschuss
IF uschuss <> y + 30 THEN PUT (x + 1, uschuss - 29), skern, PSET
DELAY 500
NEXT uschuss
xpo = x
ypo = uschuss
rpo = 3
RETURN

osch:
FOR oschuss = y - 30 TO 0 STEP -30
IF feld(x / 10, oschuss / 10) <> 0 THEN EXIT FOR
kern x, oschuss
IF oschuss <> y - 30 THEN PUT (x + 1, oschuss + 31), skern, PSET
DELAY 500
NEXT oschuss
xpo = x
ypo = oschuss
rpo = 4
RETURN

nachschuss:
IF rpo = 1 AND xpo + 30 <> x THEN PUT (xpo + 31, ypo + 1), skern, PSET
IF rpo = 2 AND xpo - 30 <> x THEN PUT (xpo - 29, ypo + 1), skern, PSET
IF rpo = 3 AND ypo - 30 <> y THEN PUT (xpo + 1, ypo - 29), skern, PSET
IF rpo = 4 AND ypo + 30 <> y THEN PUT (xpo + 1, ypo + 31), skern, PSET

SELECT CASE feld(xpo / 10, ypo / 10)

CASE 1, 31, 32, 33, 34                                    'MAUER
                                              'SCHLEUSEN
IF rpo = 1 AND xpo + 30 <> x THEN EXPLOSION xpo + 30, ypo, 9, 1
IF rpo = 2 AND xpo - 30 <> x THEN EXPLOSION xpo - 30, ypo, 9, 1
IF rpo = 3 AND ypo - 30 <> y THEN EXPLOSION xpo, ypo - 30, 9, 1
IF rpo = 4 AND ypo + 30 <> y THEN EXPLOSION xpo, ypo + 30, 9, 1

CASE 2 - 19

CASE 20                                        'MONSTER
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4
score = score + 20

CASE 21 - 23
REM

CASE 24                                         'KERN
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern

CASE 25                                         'FELS
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: BLACK xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2

CASE 26 - 30
REM

CASE ELSE
END SELECT
GOSUB kp2
RETURN

SUB BLACK (a, B, z)
COLOR 0

feld(a / 10, B / 10) = 0
ON z GOTO b1, b2, b3, b4, b5, b6, b7, b8

b1:
LINE (a + 1, B + 0)-(a + 29, B + 30), 0, BF
GOTO en

b2:
LINE (a + 0, B + 1)-(a + 30, B + 29), 0, BF
GOTO en

b3:
LINE (a + 29, B + 29)-(a + 1, B + 1), 0, BF
GOTO en

b4:
LINE (a, B)-(a + 30, B + 30), 0, BF
GOTO en

b5:
LINE (a + 1, B)-(a + 30, B + 30), 0, BF
GOTO en

b6:
LINE (a, B)-(a + 29, B + 30), 0, BF
GOTO en

b7:
LINE (a, B + 1)-(a + 30, B + 30), 0, BF
GOTO en

b8:
LINE (a, B)-(a + 30, B + 29), 0, BF
GOTO en

en:

END SUB

SUB DELAY (dauer%)
FOR i = 0 TO dauer%
NEXT i
END SUB

SUB DING (art, x, y, f)

feld(x / 10, y / 10) = art + 1
SELECT CASE art

CASE 1:

COLOR 14
CIRCLE (x + 15, y + 15), 7
PAINT (x + 15, y + 15)
CIRCLE (x + 15, y + 15), 6, 12
COLOR 0
LINE (x + 16, y + 19)-(x + 16, y + 11)
LINE (x + 15, y + 19)-(x + 15, y + 11)
PSET (x + 14, y + 11)
PSET (x + 14, y + 12)
PSET (x + 13, y + 12)
PSET (x + 13, y + 13)
PSET (x + 12, y + 13)

CASE 2:

COLOR 6
LINE (x + 10, y + 7)-(x + 20, y + 23), , B
PAINT (x + 15, y + 15)
COLOR 0
LINE (x + 11, y + 11)-(x + 19, y + 11)
LINE (x + 11, y + 15)-(x + 19, y + 15)
LINE (x + 11, y + 19)-(x + 19, y + 19)
LINE (x + 13, y + 8)-(x + 13, y + 22)
LINE (x + 17, y + 8)-(x + 17, y + 22)

CASE 3

COLOR 12
LINE (x + 15, y + 25)-(x + 5, y + 15)
LINE (x + 15, y + 25)-(x + 25, y + 15)
PSET (x + 15, y + 10)
PSET (x + 14, y + 10)
PSET (x + 13, y + 9)
PSET (x + 16, y + 9)
PSET (x + 12, y + 8)
PSET (x + 17, y + 8)
PSET (x + 18, y + 8)
PSET (x + 11, y + 8)
PSET (x + 5, y + 14)
PSET (x + 5, y + 13)
PSET (x + 5, y + 12)
PSET (x + 6, y + 11)
PSET (x + 7, y + 10)
PSET (x + 8, y + 9)
PSET (x + 9, y + 8)
PSET (x + 10, y + 7)
PSET (x + 25, y + 14)
PSET (x + 25, y + 13)
PSET (x + 25, y + 12)
PSET (x + 24, y + 11)
PSET (x + 23, y + 10)
PSET (x + 22, y + 9)
PSET (x + 21, y + 8)
PSET (x + 20, y + 7)
PSET (x + 19, y + 7)
PAINT (x + 15, y + 15)

CASE 4

COLOR 6
LINE (x + 14, y + 27)-(x + 15, y + 4), , B
LINE (x + 6, y + 11)-(x + 24, y + 12), , B

CASE 5

COLOR 15
CIRCLE (x + 15, y + 15), 14
COLOR 12
CIRCLE (x + 15, y + 15), 13
PAINT (x + 15, y + 15)
COLOR 15
CIRCLE (x + 15, y + 15), 3
PAINT (x + 15, y + 15)
COLOR 0
LINE (x + 15, y + 15)-(x + 25, y + 29)
LINE (x + 15, y + 15)-(x + 29, y + 15)
LINE (x + 25, y + 29)-(x + 29, y + 29)
LINE (x + 29, y + 29)-(x + 29, y + 15)
PAINT (x + 18, y + 18)
PSET (x + 25, y + 25)
PSET (x + 24, y + 26)
COLOR 15
PSET (x + 10, y + 10)
PSET (x + 20, y + 10)
PSET (x + 10, y + 20)
PSET (x + 15, y + 10)
PSET (x + 15, y + 20)
PSET (x + 10, y + 15)
PSET (x + 5, y + 15)
PSET (x + 15, y + 25)
PSET (x + 15, y + 5)
PSET (x + 15, y + 15)

CASE 6

COLOR 15
CIRCLE (x + 15, y + 15), 14
COLOR 12
CIRCLE (x + 15, y + 15), 13
PAINT (x + 15, y + 15)
COLOR 15
CIRCLE (x + 15, y + 15), 10
CIRCLE (x + 15, y + 15), 7
CIRCLE (x + 15, y + 15), 3
PAINT (x + 15, y + 15)
COLOR 0
LINE (x + 30, y + 30)-(x + 0, y + 30)
LINE (x + 14, y + 15)-(x + 30, y + 23)
LINE (x + 14, y + 15)-(x + 30, y + 10)
LINE (x + 30, y + 10)-(x + 30, y + 0)
LINE (x + 0, y + 0)-(x + 30, y + 0)
LINE (x + 0, y + 30)-(x + 0, y + 0)
PAINT (x + 14, y + 14), 0
PSET (x + 25, y + 5)
PSET (x + 5, y + 5)
PSET (x + 5, y + 25)
COLOR 15
LINE (x + 15, y + 15)-(x + 15, y + 26)
LINE (x + 15, y + 26)-(x + 27, y + 30)
LINE (x + 28, y + 30)-(x + 28, y + 20)
COLOR 0
PSET (x + 26, y + 24)
PSET (x + 25, 25)
LINE (x + 29, y + 15)-(x + 29, y + 29), 15
LINE (x + 16, y + 25)-(x + 27, y + 29), 6
LINE (x + 16, y + 24)-(x + 27, y + 28), 6
LINE (x + 16, y + 23)-(x + 27, y + 27), 6
LINE (x + 16, y + 20)-(x + 27, y + 24), 6
LINE (x + 16, y + 19)-(x + 27, y + 23), 6
LINE (x + 16, y + 18)-(x + 27, y + 22), 6
LINE (x + 16, y + 17)-(x + 27, y + 21), 6
LINE (x + 16, y + 22)-(x + 27, y + 26), 14
LINE (x + 16, y + 21)-(x + 27, y + 25), 14

CASE 7

COLOR f
CIRCLE (x + 15, y + 20), 6
PAINT (x + 15, y + 15)
LINE (x + 13, y + 15)-(x + 17, y + 5), , B
PAINT (x + 14, y + 11)
LINE (x + 14, y + 4)-(x + 16, y + 4), 6
LINE (x + 13, y + 3)-(x + 17, y + 3), 6

CASE 8

COLOR 15
LINE (x + 9, y + 29)-(x + 21, y + 29)
LINE (x + 9, y + 29)-(x + 9, y + 20)
LINE (x + 21, y + 29)-(x + 21, y + 20)
LINE (x + 14, y + 3)-(x + 14, y + 12)
LINE (x + 16, y + 3)-(x + 16, y + 12)
LINE (x + 14, y + 12)-(x + 9, y + 20)
LINE (x + 16, y + 12)-(x + 21, y + 20)
LINE (x + 14, y + 2)-(x + 16, y + 2), 6
LINE (x + 13, y + 1)-(x + 17, y + 1), 6
PSET (x + 15, y + 3)
PAINT (x + 15, y + 15)
COLOR 0
LINE (x + 10, y + 28)-(x + 10, y + 22)
LINE (x + 10, y + 25)-(x + 12, y + 28)
PSET (x + 11, y + 22)
PSET (x + 12, y + 22)
PSET (x + 12, y + 23)
PSET (x + 12, y + 24)
PSET (x + 12, y + 25)
PSET (x + 11, y + 25)
PSET (x + 10, y + 25)
LINE (x + 14, y + 22)-(x + 14, y + 27)
LINE (x + 16, y + 22)-(x + 16, y + 27)
LINE (x + 15, y + 28)-(x + 15, y + 28)
LINE (x + 18, y + 22)-(x + 18, y + 28)
LINE (x + 20, y + 22)-(x + 20, y + 28)
PSET (x + 19, y + 23)
PSET (x + 19, y + 24)

CASE 9

COLOR 6
LINE (x + 10, y + 15)-(x + 15, y + 30)
LINE (x + 20, y + 15)-(x + 15, y + 30)
LINE (x + 10, y + 15)-(x + 20, y + 15)
LINE (x + 12, y + 15)-(x + 17, y + 27)
LINE (x + 15, y + 15)-(x + 18, y + 24)
PSET (x + 19, y + 16)
CIRCLE (x + 15, y + 6), 4, 10
PAINT (x + 15, y + 6), 10
CIRCLE (x + 12, y + 12), 4, 12
PAINT (x + 12, y + 12), 12
COLOR 14
CIRCLE (x + 18, y + 12), 4, 14
PAINT (x + 18, y + 12)

CASE 10

COLOR 15
CIRCLE (x + 15, y + 10), 5
CIRCLE (x + 15, y + 10), 4.5, 14
PAINT (x + 15, y + 10), 14
COLOR 7
LINE (x + 13, y + 16)-(x + 17, y + 21), , B
PAINT (x + 15, y + 18)
PSET (x + 14, y + 22)
PSET (x + 15, y + 22)
PSET (x + 16, y + 22)

CASE 11

CIRCLE (x + 15, y + 15), 9, 10
PAINT (x + 15, y + 15), 10
CIRCLE (x + 15, y + 15), 10, 14
COLOR 0
LINE (x + 15, y + 15)-(x + 15, y + 23)
LINE (x + 7, y + 15)-(x + 15, y + 15)

CASE 12

COLOR 9
LINE (x + 9, y + 10)-(x + 15, y + 30)
LINE (x + 21, y + 10)-(x + 15, y + 30)
PSET (x + 10, y + 10)
PSET (x + 11, y + 11)
PSET (x + 12, y + 10)
PSET (x + 13, y + 11)
PSET (x + 14, y + 10)
PSET (x + 15, y + 11)
PSET (x + 16, y + 10)
PSET (x + 17, y + 11)
PSET (x + 18, y + 10)
PSET (x + 19, y + 11)
PSET (x + 20, y + 10)
PAINT (x + 15, y + 15)
LINE (x + 15, y + 10)-(x + 15, y + 4), 6
PSET (x + 16, y + 3), 6
PSET (x + 17, y + 2), 6
PSET (x + 18, y + 2), 6
PSET (x + 19, y + 2), 6
PSET (x + 20, y + 3), 6
PSET (x + 20, y + 4), 6

CASE 13

COLOR 15
LINE (x + 7, y + 5)-(x + 23, y + 25), , B
PAINT (x + 10, y + 10)
COLOR 0
LINE (x + 9, y + 8)-(x + 21, y + 8)
LINE (x + 9, y + 11)-(x + 21, y + 11)
LINE (x + 9, y + 14)-(x + 15, y + 14)
LINE (x + 9, y + 17)-(x + 21, y + 17)
LINE (x + 9, y + 20)-(x + 17, y + 20)
LINE (x + 15, y + 23)-(x + 19, y + 23)

CASE 14

COLOR f
LINE (x + 2, y + 10)-(x + 2, y + 20)
LINE (x + 3, y + 9)-(x + 3, y + 21)
LINE (x + 4, y + 8)-(x + 6, y + 10), , B
LINE (x + 4, y + 20)-(x + 6, y + 22), , B
PAINT (x + 5, y + 9)
PAINT (x + 5, y + 21)
LINE (x + 7, y + 9)-(x + 7, y + 21)
LINE (x + 8, y + 10)-(x + 8, y + 20)
LINE (x + 9, y + 13)-(x + 29, y + 15), , B
PAINT (x + 10, y + 14)
LINE (x + 20, y + 16)-(x + 28, y + 18), , B
PAINT (x + 22, y + 17)
PSET (x + 28, y + 19)
PSET (x + 27, y + 19)
PSET (x + 25, y + 19)
PSET (x + 24, y + 19)
PSET (x + 28, y + 20)
PSET (x + 24, y + 20)
PSET (x + 21, y + 19)
PSET (x + 25, y + 20)
PSET (x + 27, y + 20)
PSET (x + 21, y + 20)
PSET (x + 22, y + 20)
PSET (x + 22, y + 19)

CASE 15

COLOR 6
CIRCLE (x + 15, y + 15), 10
PAINT STEP(0, 0)
COLOR 0
PSET (x + 15, y + 15)
FOR Sx = 0 TO 10 STEP .1
PSET (x + 15 + Sx * COS(Sx), y + 15 + Sx * SIN(Sx))
NEXT Sx

CASE 16

COLOR 4
CIRCLE (x + 9, y + 23), 4
CIRCLE (x + 21, y + 23), 4
PAINT (x + 9, y + 23)
PAINT (x + 21, y + 23)
COLOR 2
LINE (x + 15, y + 7)-(x + 9, y + 18)
LINE (x + 15, y + 8)-(x + 10, y + 18)
LINE (x + 15, y + 7)-(x + 21, y + 18)
LINE (x + 15, y + 8)-(x + 20, y + 18)

CASE 17

COLOR 6
LINE (x + 3, y + 14)-(x + 19, y + 16), , BF
LINE (x + 4, y + 13)-(x + 4, y + 17)
LINE (x + 5, y + 13)-(x + 5, y + 17)
LINE (x + 19, y + 13)-(x + 19, y + 17)
COLOR 15
CIRCLE (x + 24, y + 15), 4
PAINT (x + 24, y + 15)

END SELECT

END SUB

SUB EIS (x, y)

feld(x / 10, y / 10) = 19
LINE (x + 0, y + 0)-(x + 30, y + 30), 15, BF

END SUB

SUB EXPLOSION (xp, yp, rad, col)

xp = xp + 15
yp = yp + 15
PLAY "MBO0L32EFGEFDC"
FOR i = 0 TO rad STEP .41
CIRCLE (xp, yp), i, col
NEXT i
FOR i = rad TO 0 STEP (-.31)
CIRCLE (xp, yp), i, 0
DELAY 250
NEXT i

END SUB

SUB FELS (x, y)

feld(x / 10, y / 10) = 25
COLOR 8
LINE (x + 0, y + 0)-(x + 30, y + 30), 8, BF
LINE (x + 2, y + 2)-(x + 28, y + 28), 7, B
LINE (x + 3, y + 3)-(x + 27, y + 27), 7, B
CIRCLE (x + 15, y + 15), 3, 7
PAINT STEP(0, 0), 7

END SUB

SUB kern (a, B)
CIRCLE (a + 15, B + 15), 3, 6
CIRCLE (a + 15, B + 15), 2.5, 6
CIRCLE (a + 15, B + 15), 2, 6
CIRCLE (a + 15, B + 15), 1.5, 6
CIRCLE (a + 15, B + 15), 1, 6
CIRCLE (a + 15, B + 15), .5, 6
PSET (a + 15, B + 15), 15

END SUB

SUB LEVELS (level%)
SELECT CASE level%
CASE 1
FOR l1z = 60 TO 570 STEP 30
CALL MAUER(l1z, 210)
NEXT l1z
DING 1, 480, 270, 0
DING 1, 480, 90, 0
CALL MAUER(570, 60)
CALL TOR(570, 30, 1)
DING 14, 570, 390, 1
x = 30
y = 30
r = 2
CASE 2
CALL TOR(30, 30, 14)
CALL WASSER(480, 30)
CALL WASSER(510, 30)
CALL WASSER(540, 30)
CALL WASSER(570, 30)
CALL WASSER(510, 60)
CALL WASSER(540, 60)
CALL WASSER(570, 60)
CALL WASSER(540, 90)
CALL WASSER(570, 90)
CALL WASSER(570, 120)
CALL WASSER(570, 330)
CALL WASSER(540, 360)
CALL WASSER(570, 360)
CALL WASSER(510, 390)
CALL WASSER(540, 390)
CALL WASSER(570, 390)
CALL WASSER(30, 300)
CALL WASSER(30, 330)
CALL WASSER(60, 330)
CALL WASSER(30, 360)
CALL WASSER(60, 360)
CALL WASSER(90, 360)
CALL WASSER(30, 390)
CALL WASSER(60, 390)
CALL WASSER(90, 390)
CALL WASSER(120, 390)
CALL WASSER(90, 90)
CALL WASSER(90, 120)
FOR l2z = 30 TO 420 STEP 30
CALL MAUER(l2z, 60)
NEXT l2z
FOR l2a = 90 TO 360 STEP 30
CALL MAUER(300, l2a)
NEXT l2a
FOR l2b = 30 TO 240 STEP 30
CALL MAUER(l2b, 150)
NEXT l2b
FOR l2c = 120 TO 360 STEP 30
CALL MAUER(360, l2c)
NEXT l2c
CALL MAUER(450, 90)
CALL MAUER(480, 120)
CALL MAUER(510, 150)
CALL MAUER(540, 180)
CALL MAUER(540, 210)
CALL MAUER(540, 240)
CALL MAUER(480, 330)
CALL MAUER(450, 360)
CALL MAUER(420, 360)
CALL MAUER(390, 360)
CALL MAUER(390, 210)
CALL MAUER(420, 210)
CALL MAUER(450, 210)
CALL MAUER(480, 210)
CALL MAUER(480, 240)
CALL MAUER(450, 270)
CALL MAUER(420, 300)
CALL MAUER(420, 270)
CALL MAUER(60, 210)
CALL MAUER(60, 240)
CALL MAUER(90, 270)
CALL MAUER(120, 300)
CALL MAUER(150, 330)
CALL MAUER(180, 360)
CALL MAUER(210, 360)
CALL MAUER(240, 360)
CALL MAUER(270, 360)
DING 1, 270, 30, 0
DING 1, 540, 60, 0
DING 1, 420, 240, 0
DING 14, 450, 240, 14
DING 14, 30, 90, 14
DING 3, 450, 30, 0
DING 3, 60, 90, 0
DING 7, 30, 120, 2
DING 1, 60, 120, 0
DING 5, 360, 390, 0
DING 2, 210, 180, 0
DING 2, 90, 300, 0
LINE (271, 60)-(299, 90), 0, BF
CALL BLACK(270, 60, 2)
CALL MAUER(540, 270)
CALL MAUER(510, 300)
x = 60
y = 30
r = 2
CASE 3
CALL TOR(30, 30, 4)
CALL TOR(120, 90, 1)
CALL TOR(570, 60, 14)
CALL TOR(270, 360, 2)
CALL TOR(330, 360, 5)
CALL MAUER(30, 60)
CALL MAUER(30, 90)
CALL MAUER(120, 30)
CALL MAUER(150, 30)
CALL MAUER(120, 60)
CALL MAUER(150, 60)
CALL MAUER(210, 60)
CALL MAUER(210, 90)
CALL MAUER(300, 30)
CALL MAUER(300, 60)
CALL MAUER(330, 60)
CALL MAUER(360, 60)
FOR l3z = 30 TO 210 STEP 30
CALL MAUER(l3z, 120)
CALL MAUER(l3z, 240)
NEXT l3z
CALL MAUER(240, 240)
CALL MAUER(270, 120)
CALL MAUER(300, 120)
CALL MAUER(330, 120)
CALL MAUER(450, 30)
CALL MAUER(450, 60)
CALL MAUER(450, 90)
CALL MAUER(450, 120)
CALL MAUER(420, 120)
CALL MAUER(390, 120)
CALL MAUER(480, 120)
CALL MAUER(510, 120)
CALL MAUER(510, 90)
CALL MAUER(510, 60)
CALL MAUER(540, 60)
CALL MAUER(570, 120)
CALL MAUER(270, 270)
CALL MAUER(270, 300)
CALL MAUER(270, 330)
CALL MAUER(330, 270)
CALL MAUER(330, 300)
CALL MAUER(330, 330)
CALL MAUER(390, 330)
CALL MAUER(420, 330)
CALL MAUER(540, 300)
CALL MAUER(540, 330)
CALL MAUER(540, 360)
FOR l3a = 360 TO 570 STEP 30
CALL MAUER(l3a, 240)
NEXT l3a
CALL WASSER(30, 180)
CALL WASSER(300, 180)
CALL WASSER(540, 180)
CALL WASSER(270, 240)
CALL WASSER(330, 240)
CALL WASSER(150, 330)
CALL WASSER(150, 360)
FOR l3b = 210 TO 570 STEP 30
CALL WASSER(l3b, 390)
NEXT l3b
FOR l3c = 60 TO 240 STEP 30
CALL WASSER(l3c, 270)
NEXT l3c
FOR l3d = 360 TO 570 STEP 30
CALL WASSER(l3d, 270)
NEXT l3d
CALL WASSER(570, 300)
CALL WASSER(570, 330)
CALL WASSER(570, 360)
CALL WASSER(360, 300)
CALL WASSER(360, 330)
CALL WASSER(60, 330)
CALL WASSER(60, 360)
CALL WASSER(60, 390)
CALL WASSER(210, 330)
CALL WASSER(240, 330)
CALL WASSER(120, 330)
CALL WASSER(120, 360)
FOR l3e = 120 TO 240 STEP 30
CALL WASSER(l3e, 300)
NEXT l3e
CALL TISCH(510, 300, 2)
DING 1, 60, 30, 0
DING 1, 90, 30, 0
DING 1, 60, 60, 0
DING 1, 90, 60, 0
DING 1, 60, 90, 0
DING 1, 90, 90, 0
DING 1, 480, 60, 0
DING 1, 30, 300, 0
DING 1, 30, 330, 0
DING 1, 30, 360, 0
DING 1, 90, 330, 0
DING 1, 90, 360, 0
DING 8, 270, 30, 0
DING 9, 390, 60, 0
DING 6, 420, 300, 0
CALL MAUER(420, 330)
CALL WASSER(420, 270)
 DING 2, 390, 300, 0
 DING 5, 180, 330, 0
 DING 14, 30, 390, 4
 DING 14, 510, 330, 1
 DING 14, 330, 30, 14
 DING 14, 480, 90, 2
DING 14, 30, 270, 5
feld(57, 12) = 1
feld(39, 30) = 3
feld(51, 33) = 1
x = 150
y = 90
r = 2
CASE 4
FOR l4z = 30 TO 540 STEP 30
CALL MAUER(l4z, 60)
CALL MAUER(l4z, 180)
CALL MAUER(l4z, 300)
NEXT l4z
FOR l4a = 60 TO 600 STEP 30
CALL MAUER(l4a, 120)
CALL MAUER(l4a, 240)
CALL MAUER(l4a, 360)
NEXT l4a
CALL TOR(30, 30, 4)
DING 14, 570, 390, 4
 DING 1, 570, 60, 0
 DING 1, 570, 180, 0
 DING 1, 570, 300, 0
 DING 1, 30, 360, 0
 DING 1, 30, 240, 0
 DING 1, 30, 120, 0
 DING 2, 300, 270, 0
 DING 2, 300, 150, 0
x = 60
y = 30
r = 2
CASE 5
 DING 1, 120, 60, 0
 DING 1, 540, 60, 0
 DING 1, 120, 360, 0
 DING 1, 540, 360, 0
 DING 3, 270, 210, 0
CALL TOR(30, 360, 2)
 DING 14, 30, 60, 2
 DING 2, 30, 30, 0
 DING 5, 30, 270, 0
FOR l5z = 30 TO 390 STEP 30
CALL MAUER(60, l5z)
NEXT l5z
CALL MAUER(30, 390)
CALL MAUER(30, 120)
CALL MAUER(30, 180)
CALL SCHLEUSE(30, 120, 4)
CALL SCHLEUSE(30, 180, 3)
 DING 7, 510, 360, 4
 DING 15, 330, 210, 0
feld(6, 3) = 31
feld(6, 27) = 31
feld(3, 12) = 33
feld(3, 18) = 34
x = 300
y = 210
r = 3
CASE 6
 DING 16, 510, 30, 0
FOR l6z = 30 TO 570 STEP 60
CALL MAUER(l6z, 60)
CALL MAUER(l6z, 150)
CALL MAUER(l6z, 270)
CALL WASSER(l6z, 210)
CALL WASSER(l6z, 330)
NEXT l6z
FOR l6a = 60 TO 540 STEP 60
CALL MAUER(l6a, 60)
CALL WASSER(l6a, 150)
CALL WASSER(l6a, 270)
CALL MAUER(l6a, 210)
CALL MAUER(l6a, 330)
NEXT l6a
CALL TOR(90, 30, 12)
CALL TOR(210, 30, 9)
CALL TOR(330, 30, 5)
CALL TOR(570, 30, 10)
CALL TOR(240, 150, 4)
CALL TOR(300, 150, 1)
CALL TOR(360, 150, 14)
CALL TOR(270, 210, 2)
CALL TOR(330, 210, 4)
CALL TOR(240, 270, 1)
CALL TOR(300, 270, 14)
CALL TOR(360, 270, 2)
CALL TOR(270, 330, 4)
CALL TOR(330, 330, 1)
 DING 14, 30, 390, 4
 DING 14, 90, 390, 1
 DING 14, 150, 390, 14
 DING 14, 210, 390, 2
 DING 14, 270, 390, 12
 DING 14, 330, 390, 9
 DING 14, 390, 390, 5
 DING 14, 450, 390, 10
 DING 6, 510, 390, 0
 DING 3, 570, 390, 0
 DING 1, 60, 30, 0
 DING 1, 150, 30, 0
 DING 1, 270, 30, 0
 DING 1, 390, 30, 0
 DING 1, 450, 30, 0
LINE (31, 60)-(59, 90), 0, BF
feld(3, 6) = 0
x = 30
y = 360
r = 2
CASE 7
FOR l7z = 150 TO 270 STEP 30
CALL MAUER(l7z, 150)
CALL MAUER(l7z, 240)
NEXT l7z
FOR l7a = 360 TO 450 STEP 30
CALL MAUER(l7a, 150)
CALL MAUER(l7a, 240)
NEXT l7a
FOR l7c = 30 TO 270 STEP 30
CALL MAUER(l7c, 60)
CALL MAUER(l7c, 330)
NEXT l7c
FOR l7d = 360 TO 570 STEP 30
CALL MAUER(l7d, 60)
NEXT l7d
CALL MAUER(270, 90)
CALL MAUER(270, 120)
CALL MAUER(300, 120)
CALL MAUER(360, 90)
CALL MAUER(360, 120)
CALL MAUER(270, 270)
CALL MAUER(270, 300)
CALL MAUER(270, 360)
CALL MAUER(270, 390)
CALL MAUER(30, 360)
CALL MAUER(90, 390)
CALL MAUER(150, 360)
CALL MAUER(210, 390)
CALL MAUER(30, 150)
CALL MAUER(60, 150)
CALL MAUER(30, 240)
CALL MAUER(60, 240)
CALL MAUER(540, 150)
CALL MAUER(570, 150)
CALL MAUER(540, 240)
CALL MAUER(570, 240)
FOR l7b = 270 TO 390 STEP 30
CALL MAUER(360, l7b)
NEXT l7b
 DING 14, 570, 30, 2
 DING 14, 30, 120, 4
 DING 14, 30, 210, 5
 DING 14, 390, 120, 14
 DING 14, 570, 210, 3
 DING 14, 390, 390, 1
CALL TOR(330, 120, 2)
CALL TOR(30, 390, 1)
CALL TOR(90, 360, 4)
CALL TOR(150, 390, 14)
CALL TOR(210, 360, 3)
CALL TOR(240, 330, 5)
CALL TISCH(450, 330, 1)
 DING 1, 120, 90, 0
 DING 1, 120, 120, 0
 DING 1, 120, 180, 0
 DING 1, 120, 210, 0
 DING 1, 300, 390, 0
 DING 1, 330, 390, 0
 DING 1, 420, 330, 0
 DING 1, 510, 330, 0
 DING 16, 450, 90, 0
 DING 16, 450, 120, 0
DING 6, 450, 180, 0
DING 2, 450, 210, 0
CALL FELS(60, 30)
CALL FELS(540, 30)
feld(15, 36) = 1
wwasser(1) = 300
wwasser(2) = 180
wwasser(3) = 210
wwasser(4) = 120
wwasser(5) = 480
wwasser(6) = 510
wwasser(7) = 90
x = 30
y = 30
r = 2
CASE 8
FOR l8z = 30 TO 240 STEP 30
CALL MAUER(l8z, 90)
CALL WASSER(l8z, 210)
CALL WASSER(330 + l8z, 210)
CALL MAUER(30 + l8z, 150)
NEXT l8z
FOR l8a = 30 TO 120 STEP 30
CALL WASSER(300, l8a)
CALL WASSER(300, 270 + l8a)
NEXT l8a
CALL TOR(30, 390, 1)
CALL TOR(270, 30, 4)
CALL TOR(570, 30, 14)
CALL TOR(330, 390, 2)
CALL MAUER(240, 30)
CALL MAUER(60, 390)
CALL MAUER(360, 390)
CALL MAUER(570, 60)
CALL WASSER(240, 150)
CALL WASSER(270, 150)
CALL MAUER(300, 210)
CALL MAUER(240, 210)
CALL MAUER(360, 210)
CALL MAUER(300, 150)
CALL MAUER(300, 270)
CALL MAUER(210, 150)
CALL WASSER(330, 60)
CALL WASSER(360, 60)
CALL WASSER(510, 150)
CALL WASSER(540, 150)
CALL WASSER(570, 150)
CALL TISCH(240, 390, 1)
CALL WASSER(360, 120)
CALL WASSER(360, 150)
CALL WASSER(360, 180)
CALL WASSER(390, 120)
CALL WASSER(450, 120)
CALL MAUER(450, 150)
CALL MAUER(450, 180)
DING 7, 420, 90, 13
CALL FELS(300, 180)
CALL FELS(300, 240)
CALL FELS(270, 210)
CALL FELS(330, 210)
DING 1, 420, 120, 0
DING 1, 420, 150, 0
DING 1, 420, 180, 0
DING 1, 390, 150, 0
DING 1, 390, 180, 0
DING 1, 570, 270, 0
DING 1, 570, 300, 0
DING 1, 570, 330, 0
DING 1, 570, 360, 0
DING 3, 570, 390, 0
DING 17, 60, 180, 0
DING 5, 510, 180, 0
CALL FELS(300, 180)
DING 14, 30, 240, 14
DING 14, 330, 30, 4
DING 14, 570, 180, 2
DING 14, 570, 240, 1
CALL WASSER(90, 270)
CALL WASSER(90, 300)
CALL WASSER(150, 330)
CALL WASSER(150, 360)
CALL WASSER(210, 300)
CALL WASSER(210, 330)
CALL WASSER(540, 270)
CALL WASSER(540, 300)
CALL WASSER(540, 360)
CALL WASSER(540, 330)
wwasser(1) = 300
wwasser(2) = 330
wwasser(3) = 330
wwasser(4) = 390
x = 30
y = 30
r = 2
CASE 9:
FOR l9z = 90 TO 330 STEP 30
CALL WASSER(60, l9z)
CALL WASSER(120, l9z)
CALL WASSER(180, l9z)
CALL WASSER(240, l9z)
CALL WASSER(300, l9z)
CALL WASSER(360, l9z)
CALL WASSER(420, l9z)
CALL WASSER(480, l9z)
CALL WASSER(540, l9z)
NEXT l9z
CALL TOR(570, 30, 11)
DING 14, 330, 390, 11
CALL MAUER(570, 60)
wwasser(1) = 30
wwasser(2) = 360
x = 30
y = 30
r = 2
CASE 10
FOR l10z = 60 TO 150 STEP 30
FOR l10a = 30 TO 390 STEP 30
CALL WASSER(l10z, l10a)
NEXT l10a
NEXT l10z
FOR l10b = 210 TO 540 STEP 30
CALL MAUER(l10b, 60)
CALL MAUER(l10b, 120)
CALL MAUER(l10b, 180)
CALL MAUER(l10b, 240)
CALL MAUER(l10b, 300)
CALL MAUER(l10b, 360)
NEXT l10b
CALL MAUER(540, 90)
CALL MAUER(540, 150)
CALL MAUER(540, 210)
CALL MAUER(540, 270)
CALL MAUER(540, 330)
CALL MAUER(180, 60)
CALL MAUER(210, 330)
CALL MAUER(210, 150)
CALL MAUER(210, 210)
CALL MAUER(210, 270)
CALL BLACK(510, 120, 1)
CALL BLACK(510, 180, 1)
CALL BLACK(510, 240, 1)
CALL BLACK(510, 300, 1)
CALL BLACK(240, 180, 1)
CALL BLACK(240, 240, 1)
CALL BLACK(240, 300, 1)
CALL MAUER(480, 150)
CALL MAUER(480, 210)
CALL MAUER(480, 270)
CALL MAUER(270, 210)
CALL MAUER(270, 270)
CALL MAUER(420, 210)
CALL BLACK(450, 180, 1)
CALL BLACK(450, 240, 1)
CALL BLACK(300, 240, 1)
CALL TOR(540, 360, 13)
DING 14, 390, 210, 13
DING 17, 210, 30, 0
CALL FELS(240, 30)
DING 3, 30, 30, 0
CALL FELS(30, 60)
DING 5, 30, 90, 0
DING 5, 30, 150, 0
DING 16, 30, 120, 0
DING 16, 30, 270, 0
DING 9, 30, 180, 0
DING 9, 30, 210, 0
DING 9, 30, 240, 0
DING 1, 30, 300, 0
DING 2, 30, 330, 0
DING 2, 30, 360, 0
DING 2, 30, 390, 0
FOR l10a = 60 TO 150 STEP 30
BLACK l10a, 150, 4
feld(l10a / 10, 15) = 0
NEXT l10a
x = 180
y = 30
r = 2
CASE 11
FOR l11z = 30 TO 570 STEP 30
FOR l11a = 30 TO 390 STEP 30
DING 1, l11z, l11a, 0
NEXT l11a
NEXT l11z
CALL MAUER(330, 180)
CALL MAUER(360, 180)
CALL MAUER(390, 180)
CALL MAUER(330, 240)
CALL MAUER(360, 240)
CALL MAUER(390, 240)
CALL MAUER(390, 210)
CALL BLACK(330, 210, 2)
CALL TOR(360, 210, 11)
CALL BLACK(300, 210, 1)
DING 14, 300, 210, 11
x = 330
y = 210
r = 1
CASE 12
WASSER 30, 30
WASSER 60, 30
WASSER 90, 30
WASSER 120, 30
WASSER 30, 60
WASSER 60, 60
WASSER 90, 60
WASSER 30, 90
WASSER 60, 90
WASSER 30, 120
WASSER 300, 30
WASSER 330, 30
WASSER 360, 30
WASSER 390, 30
WASSER 420, 30
WASSER 450, 30
WASSER 480, 30
WASSER 510, 30
WASSER 540, 30
WASSER 570, 30
WASSER 330, 60
WASSER 360, 60
WASSER 390, 60
WASSER 420, 60
WASSER 450, 60
WASSER 480, 60
WASSER 510, 60
WASSER 540, 60
WASSER 570, 60
WASSER 360, 90
WASSER 390, 90
WASSER 420, 90
WASSER 450, 90
WASSER 480, 90
WASSER 510, 90
WASSER 540, 90
WASSER 570, 90
WASSER 390, 120
WASSER 420, 120
WASSER 450, 120
WASSER 480, 120
WASSER 510, 120
WASSER 540, 120
WASSER 570, 120
WASSER 420, 150
WASSER 450, 150
WASSER 480, 150
WASSER 510, 150
WASSER 540, 150
WASSER 570, 150
WASSER 450, 180
WASSER 480, 180
WASSER 510, 180
WASSER 540, 180
WASSER 570, 180
WASSER 480, 210
WASSER 510, 210
WASSER 540, 210
WASSER 570, 210
WASSER 510, 240
WASSER 540, 240
WASSER 570, 240
WASSER 540, 270
WASSER 570, 270
WASSER 570, 300
WASSER 30, 240
WASSER 30, 270
WASSER 60, 270
WASSER 30, 300
WASSER 60, 300
WASSER 90, 300
WASSER 30, 330
WASSER 60, 330
WASSER 90, 330
WASSER 120, 330
WASSER 30, 360
WASSER 60, 360
WASSER 90, 360
WASSER 120, 360
WASSER 150, 360
WASSER 30, 390
WASSER 60, 390
WASSER 90, 390
WASSER 120, 390
WASSER 150, 390
WASSER 180, 390
WASSER 240, 390
WASSER 270, 390
WASSER 300, 390
WASSER 330, 390
WASSER 270, 360
WASSER 300, 360
WASSER 480, 300
WASSER 450, 330
WASSER 480, 330
WASSER 510, 330
WASSER 420, 360
WASSER 450, 360
WASSER 480, 360
WASSER 510, 360
WASSER 420, 390
WASSER 450, 390
WASSER 480, 390
WASSER 510, 390
WASSER 390, 390
WASSER 570, 180
MAUER 540, 390
MAUER 540, 360
WASSER 270, 180
WASSER 300, 180
WASSER 330, 180
WASSER 300, 150
WASSER 240, 210
WASSER 270, 210
WASSER 330, 210
WASSER 360, 210
WASSER 270, 240
WASSER 300, 240
WASSER 330, 240
WASSER 300, 270
FOR eisl12 = 90 TO 180 STEP 30
EIS eisl12, 180
EIS eisl12, 210
NEXT eisl12
EIS 210, 180
EIS 180, 60
EIS 240, 60
EIS 180, 90
EIS 210, 90
EIS 240, 90
DING 7, 210, 60, 5
DING 3, 150, 30, 0
DING 3, 210, 30, 0
DING 3, 270, 30, 0
DING 1, 180, 30, 0
DING 1, 240, 30, 0
DING 2, 120, 60, 0
DING 2, 90, 90, 0
DING 2, 60, 120, 0
DING 14, 30, 180, 2
DING 9, 390, 360, 0
DING 9, 420, 330, 0
DING 9, 450, 300, 0
DING 9, 480, 270, 0
TOR 570, 390, 2
MONSTER 210, 390, 3, 2: feld(21, 39) = 20
MONSTER 330, 150, 2, 3: feld(33, 15) = 20
MONSTER 210, 210, 3, 1: feld(21, 21) = 20
MONSTER 240, 180, 2, 2: feld(24, 18) = 20
MONSTER 390, 210, 1, 3: feld(39, 21) = 20
MONSTER 240, 240, 3, 4: feld(24, 24) = 20
MONSTER 300, 120, 3, 4: feld(30, 12) = 20
MONSTER 300, 300, 1, 4: feld(30, 30) = 20
MONSTER 270, 270, 2, 4: feld(27, 27) = 20
MONSTER 270, 150, 2, 2: feld(27, 15) = 20
MONSTER 360, 180, 3, 3: feld(36, 18) = 20
MONSTER 390, 210, 1, 3: feld(39, 21) = 20
MONSTER 360, 240, 3, 1: feld(36, 24) = 20
MONSTER 330, 270, 3, 1: feld(33, 27) = 20
MONSTER 570, 330, 3, 4: feld(57, 33) = 20
MONSTER 360, 390, 3, 2: feld(36, 39) = 20
kern 540, 330
OPEN "timer12.da1" FOR INPUT AS #1
FOR i = 1 TO 14
INPUT #1, viehx(i), viehy(i)
kern viehx(i), viehy(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 30
y = 150
r = 2
CASE 13
FOR l13a = 30 TO 570 STEP 30
MAUER l13a, 300
NEXT l13a
CALL MAUER(30, 390)
CALL MAUER(30, 330)
CALL TRESOR(30, 360, 3)
CALL SCHLEUSE(60, 300, 3)
CALL TOR(30, 270, 14)
CALL MAUER(420, 240)
CALL MAUER(420, 270)
CALL SCHLEUSE(420, 270, 1)
CALL MAUER(180, 150)
CALL SCHLEUSE(180, 150, 4)
CALL MAUER(390, 240)
CALL MAUER(360, 240)
CALL MAUER(330, 240)
FOR l13a = 30 TO 270 STEP 30
CALL MAUER(90, l13a)
CALL MAUER(150, l13a)
CALL MAUER(330, l13a)
CALL MAUER(570, l13a)
NEXT l13a
CALL MAUER(150, 30)
CALL MAUER(120, 240)
CALL MAUER(120, 90):
FOR l13b = 0 TO 120 STEP 30
FELS 180 + l13b, 30
FELS 120 + l13b, 120
FELS 390 + l13b, 120
FELS 360 + l13b, 210
MAUER 330 + l13b, 30
MAUER 420 + l13b, 150
NEXT l13b
FELS 120, 30
FOR l13c = 0 TO 60 STEP 30
FELS 120 + l13c, 60
MAUER 210 + l13c, 60
FELS 120, 150 + l13c
FELS 120 + l13c, 270
FELS 180 + l13c, 180
FELS 300 + l13c, 270
FELS 180 + l13c, 240
FELS 480 + l13c, 60
NEXT l13c
FOR l13d = 0 TO 30 STEP 30
MAUER 180 + l13d, 90
MAUER 210 + l13d, 150
MAUER 180 + l13d, 210
MAUER 210 + l13d, 270
FELS 270, 240 + l13d
MAUER 510 + l13d, 30
MAUER 450, 60 + l13d
MAUER 480 + l13d, 90
FELS 540, 90 + l13d
NEXT l13d
FELS 240, 90
FELS 240, 210
MAUER 270, 90
MAUER 270, 120
MAUER 270, 180
MAUER 270, 210
MAUER 540, 180
MAUER 540, 240
MAUER 570, 240
MAUER 570, 270
FELS 270, 150
FELS 300, 120
FELS 300, 150
FELS 300, 180
FELS 360, 60
FELS 360, 150
FELS 360, 180
FELS 390, 60
FELS 390, 90
FELS 390, 150
FELS 390, 180
FELS 420, 60
FELS 420, 90
FELS 420, 180
FELS 450, 180
FELS 450, 240
FELS 450, 270
FELS 480, 240
FELS 480, 270
MAUER 480, 180
FELS 510, 180
FELS 510, 210
FELS 510, 240
FELS 510, 270
FELS 540, 210
FELS 570, 210
FELS 540, 270
BLACK 180, 120, 3
BLACK 540, 270, 3
MAUER 300, 240
MAUER 300, 90
MAUER 360, 90
MAUER 360, 120
FOR l13e = 90 TO 360 STEP 30
DING 16, l13e, 330, 0
DING 16, l13e, 360, 0
DING 16, l13e, 390, 0
NEXT l13e
FOR l13f = 30 TO 180 STEP 30
DING 1, 30, l13f, 0
DING 9, 60, l13f, 0
NEXT l13f
DING 1, 420, 330, 0
DING 1, 420, 360, 0
DING 1, 420, 390, 0
DING 3, 480, 330, 0
DING 3, 480, 390, 0
WASSER 120, 210: feld(12, 21) = 27
DING 13, 480, 360, 0
feld(30, 9) = 33
feld(30, 24) = 33
feld(9, 6) = 32
feld(54, 30) = 33
feld(33, 21) = 32
x = 30
y = 240
r = 2
feld(3, 36) = 23
feld(6, 30) = 34
feld(54, 30) = 33
feld(30, 24) = 33
feld(30, 9) = 33
feld(18, 15) = 33
feld(42, 27) = 32
feld(9, 6) = 32
CASE 14
FOR l14z = 0 TO 90 STEP 30
MAUER 30 + l14z, 120
MAUER 480 + l14z, 120
MAUER 30 + l14z, 300
MAUER 480 + l14z, 300
MAUER 180 + l14z, 150
MAUER 330 + l14z, 150
MAUER 180, 180 + l14z
MAUER 420, 180 + l14z
MAUER 180 + l14z, 270
MAUER 300 + l14z, 270
MAUER 240 + l14z, 210
NEXT l14z
MAUER 360, 210
MAUER 300, 120
FOR l14a = 30 TO 90 STEP 30
MAUER 120, l14a
MAUER 480, l14a
MAUER 120, 300 + l14a
MAUER 480, 300 + l14a
NEXT l14a
TOR 300, 150, 14
TOR 210, 210, 1
TOR 390, 210, 2
TOR 300, 240, 4
DING 14, 60, 60, 1
DING 14, 60, 360, 2
DING 14, 540, 60, 4
DING 14, 540, 360, 14
DING 1, 60, 210, 0
DING 1, 90, 210, 0
DING 1, 510, 210, 0
DING 1, 540, 210, 0
DING 1, 270, 180, 0
DING 1, 330, 180, 0
DING 7, 240, 60, 9
DING 7, 300, 60, 10
DING 7, 360, 60, 11
DING 7, 420, 60, 2
DING 10, 300, 300, 0
DING 16, 210, 60, 0
DING 16, 270, 60, 0
DING 16, 330, 60, 0
DING 16, 390, 60, 0
viehx(1) = 30
viehy(1) = 30
viehr(1) = 3
viehx(2) = 510
viehy(2) = 30
viehr(2) = 3
viehx(3) = 30
viehy(3) = 330
viehr(3) = 3
viehx(4) = 510
viehy(4) = 330
viehr(4) = 3
MONSTER viehx(1), viehy(1), 3, 3: feld(viehx(1) / 10, viehy(1) / 10) = 20
MONSTER viehx(2), viehy(2), 3, 3: feld(viehx(2) / 10, viehy(2) / 10) = 20
MONSTER viehx(3), viehy(3), 3, 3: feld(viehx(3) / 10, viehy(3) / 10) = 20
MONSTER viehx(4), viehy(4), 3, 3: feld(viehx(4) / 10, viehy(4) / 10) = 20
x = 300: y = 330: r = 3
CASE 15
FOR x15 = 0 TO 180 STEP 30
WASSER 90 + x15, 60
WASSER 300 + x15, 60
WASSER 60, 60 + x15
WASSER 510, 60 + x15
WASSER 180 + x15, 300
MAUER 60 + x15, 360
MAUER 270 + x15, 360
MAUER 390 + x15, 360
NEXT x15
FELS 30, 360
MAUER 120, 300
MAUER 90, 300
WASSER 60, 330
WASSER 60, 300
WASSER 60, 270
WASSER 510, 300
WASSER 510, 270
WASSER 450, 300
WASSER 480, 300
MAUER 480, 270
MAUER 450, 240
MAUER 450, 210
MAUER 450, 180
DING 16, 480, 210, 0
DING 16, 300, 390, 0
DING 2, 270, 120, 0
DING 2, 270, 180, 0
DING 1, 240, 150, 0
DING 1, 300, 150, 0
DING 9, 270, 150, 0
DING 14, 120, 90, 4
DING 14, 150, 90, 2
TOR 480, 180, 4
TOR 480, 240, 2
FELS 120, 330
MAUER 150, 300
DING 7, 90, 330, 11
wmauer(1) = 1
wmauer(2) = 0
wmauer(3) = 1
MAUER 390, 270
MAUER 390, 300
MAUER 420, 300
MAUER 420, 330
MAUER 540, 300
MAUER 540, 330
OPEN "timer15.da2" FOR INPUT AS #2
FOR i = 1 TO 14
INPUT #2, viehx(i), viehy(i), viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #2
FOR i = 120 TO 300 STEP 30
MAUER 360, i
NEXT i
BLACK 480, 210, 3
x = 330: y = 390: r = 1
CASE 16
FOR x16 = 0 TO 180 STEP 30
MAUER 30, 120 + x16
MAUER 60, 120 + x16: feld(6, 12 + (x16 / 10)) = 19
feld(6, 30) = 34
MAUER 90, 90 + x16
MAUER 300, 30 + x16
MAUER 300, 180 + x16
MAUER 510, 120 + x16
MAUER 570, 120 + x16
MAUER 540, 120 + x16: feld(54, 12 + (x16 / 10)) = 19
feld(54, 12) = 33
MAUER 270 + x16, 360
NEXT x16
BLACK 270, 360, 6
FOR x16 = 0 TO 30 STEP 30
MAUER 150 + x16, 60
MAUER 210 + x16, 60
MAUER 180 + x16, 90
MAUER 180 + x16, 120
MAUER 510 + x16, 360
MAUER 90 + x16, 300
MAUER 510 + x16, 60
MAUER 570, 60 + x16
MAUER 510 + x16, 360
MAUER 330 + x16, 60
WASSER 360 + x16, 120
WASSER 390 + x16, 180
WASSER 420 + x16, 240
WASSER 390 + x16, 300
NEXT x16
MAUER 30, 60
MAUER 90, 30
MAUER 120, 120
MAUER 120, 180
MAUER 120, 240
MAUER 270, 120
MAUER 270, 180
MAUER 270, 240
MAUER 270, 300
MAUER 330, 180
MAUER 330, 120
MAUER 330, 240
MAUER 330, 300
MAUER 480, 120
MAUER 480, 180
MAUER 480, 240
MAUER 480, 300
MAUER 510, 330
DING 1, 60, 60, 0
DING 1, 30, 90, 0
DING 1, 120, 30, 0
DING 1, 120, 90, 0
DING 14, 570, 30, 10
TOR 30, 30, 10
wwasser(1) = 240
wwasser(2) = 180
wwasser(3) = 120
wwasser(4) = 450
wwasser(5) = 390
wwasser(6) = 360
wwasser(7) = 390
wwasser(8) = 330
OPEN "timer16.da4" FOR INPUT AS #4
FOR i = 1 TO 10
INPUT #4, viehx(i)
INPUT #4, viehy(i)
INPUT #4, viehr(i)
MONSTER viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #4
x = 60: y = 30: r = 3
CASE 17
FOR i = 0 TO 240 STEP 30
MAUER 60 + i, 60
MAUER 300 + i, 60
MAUER 60, 90 + i
MAUER 60 + i, 360
MAUER 300 + i, 360
MAUER 90 + i, 210
MAUER 300 + i, 210
MAUER 540, 90 + i
NEXT i
DING 1, 150, 240, 0
DING 1, 150, 270, 0
DING 1, 150, 300, 0
DING 9, 90, 90, 0
DING 9, 90, 330, 0
DING 1, 120, 90, 0
DING 1, 150, 90, 0
DING 1, 90, 120, 0
DING 1, 90, 150, 0
DING 1, 90, 270, 0
DING 1, 90, 300, 0
DING 1, 120, 330, 0
DING 1, 150, 330, 0
DING 1, 450, 180, 0
DING 1, 480, 180, 0
DING 1, 510, 180, 0
DING 1, 450, 240, 0
DING 1, 480, 240, 0
DING 1, 510, 240, 0
DING 3, 510, 150, 0
DING 3, 510, 270, 0
DING 2, 510, 300, 0
DING 2, 510, 120, 0
DING 14, 300, 150, 11
TOR 510, 330, 11
SCHLEUSE 60, 180, 1
SCHLEUSE 60, 240, 2
SCHLEUSE 300, 210, 4
feld(6, 18) = 32
feld(6, 24) = 31
feld(30, 21) = 33
OPEN "timer17.da2" FOR INPUT AS #2
FOR i = 1 TO 8
INPUT #2, viehx(i)
INPUT #2, viehy(i)
INPUT #2, viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #2
FOR i = 1 TO 20
wwasser(i) = 30
wwassery(i) = 30
NEXT i
x = 510: y = 90: r = 1
CASE 18
FOR i = 0 TO 120 STEP 30
MAUER 30 + i, 210
MAUER 150 + i, 210
MAUER 270, 150 + i
MAUER 330, 150 + i
MAUER 330 + i, 210
MAUER 480 + i, 210
NEXT i
FOR i = 0 TO 30 STEP 30
MAUER 390 + i, 270
MAUER 390 + i, 330
MAUER 480 + i, 270
MAUER 510, 300 + i
DING 15, 90 + i, 120, 0
DING 15, 150, 120 + i, 0
DING 15, 120 + i, 180, 0
DING 15, 90, 150 + i, 0
DING 1, 390 + i, 30, 0
DING 1, 390 + i, 60, 0
DING 1, 480 + i, 30, 0
DING 1, 480 + i, 60, 0
DING 1, 390 + i, 120, 0
DING 1, 390 + i, 150, 0
DING 1, 480 + i, 120, 0
DING 1, 480 + i, 150, 0
DING 2, 390 + i, 90, 0
DING 2, 480 + i, 90, 0
DING 2, 450, 30 + i, 0
DING 2, 450, 120 + i, 0
NEXT i
DING 2, 450, 90, 0
TOR 210, 330, 14
MAUER 390, 300
DING 7, 30, 330, 12
OPEN "timer18.da1" FOR INPUT AS #1
FOR i = 1 TO 10
INPUT #1, viehx(i)
INPUT #1, viehy(i)
INPUT #1, viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 120: y = 150: r = 3
CASE 19
FOR i = 0 TO 300 STEP 30
MAUER 150 + i, 150
MAUER 150 + i, 270
NEXT i
FOR i = 0 TO 60 STEP 60
MAUER 180 + i, 120
MAUER 300 + i, 120
MAUER 180 + i, 300
MAUER 360 + i, 300
DING 1, 510 + i, 30, 0
DING 1, 510 + i, 90, 0
DING 1, 30 + i, 330, 0
DING 1, 30 + i, 390, 0
DING 1, 510 + i, 330, 0
DING 1, 510 + i, 390, 0
DING 1, 150 + i, 120, 0
DING 1, 270 + i, 120, 0
DING 1, 390 + i, 120, 0
DING 1, 150 + i, 300, 0
DING 1, 270 + i, 300, 0
DING 1, 390 + i, 300, 0
NEXT i
DING 7, 60, 60, 9
DING 14, 540, 360, 11
DING 15, 60, 360, 0
DING 3, 300, 210, 0
TOR 180, 210, 11
FELS 300, 270
MAUER 420, 120
MAUER 300, 300
MAUER 450, 180
MAUER 450, 210
MAUER 450, 240
MAUER 150, 180
MAUER 150, 210
MAUER 150, 240
OPEN "timer19.da1" FOR INPUT AS #1
FOR i = 1 TO 29
INPUT #1, viehx(i)
INPUT #1, viehy(i)
INPUT #1, viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 540: y = 60: r = 2
CASE 20
FOR i = 0 TO 120 STEP 30
MAUER 150, 90 + i
MAUER 30 + i, 270
MAUER 210 + i, 270
MAUER 450 + i, 60
MAUER 210, 90 + i
NEXT i
FOR i = 0 TO 90 STEP 30
MAUER 360, 120 + i
MAUER 180, 270 + i
MAUER 540, 120 + i
MAUER 480, 180 + i
MAUER 390 + i, 330
MAUER 450 + i, 390
NEXT i
FOR i = 0 TO 60 STEP 30
MAUER 30 + i, 150
MAUER 150 + i, 30
MAUER 330 + i, 60
MAUER 420 + i, 120
MAUER 270, 330 + i
MAUER 330, 300 + i
MAUER 120, 330 + i
MAUER 270, 60 + i
MAUER 60 + i, 210
NEXT i
FOR i = 0 TO 30 STEP 30
MAUER 60 + i, 60
MAUER 60 + i, 330
MAUER 240, 330 + i
MAUER 300, 210 + i
MAUER 420, 240 + i
MAUER 390, 360 + i
MAUER 540, 330 + i
MAUER 540 + i, 270
NEXT i
MAUER 90, 90
MAUER 210, 60
MAUER 90, 360
MAUER 30, 390
MAUER 300, 120
MAUER 330, 30
MAUER 450, 270
MAUER 390, 180
DING 1, 30, 120, 0
DING 14, 60, 90, 9
TOR 570, 330, 9
DING 14, 570, 30, 10
TOR 570, 390, 10
DING 14, 90, 390, 11
TOR 450, 30, 11
DING 1, 30, 360, 0
DING 1, 150, 390, 0
DING 1, 300, 390, 0
DING 1, 510, 360, 0
DING 1, 210, 330, 0
DING 1, 450, 300, 0
DING 1, 390, 270, 0
DING 1, 120, 240, 0
DING 1, 270, 240, 0
DING 1, 450, 240, 0
DING 1, 570, 240, 0
DING 1, 180, 150, 0
DING 1, 300, 150, 0
DING 1, 420, 150, 0
DING 1, 510, 180, 0
DING 1, 240, 90, 0
DING 1, 300, 90, 0
DING 1, 450, 90, 0
DING 1, 390, 30, 0
DING 3, 480, 150, 0
DING 3, 120, 90, 0
OPEN "timer20.da1" FOR INPUT AS #1
FOR i = 1 TO 20
INPUT #1, viehx(i)
INPUT #1, viehy(i)
INPUT #1, viehr(i)
MONSTER viehx(i), viehy(i), 1, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 240: y = 180: r = 3
CASE 21
FOR i = 0 TO 120 STEP 30
MAUER 150, 60 + i: MAUER 150, 240 + i
MAUER 60 + i, 210
MAUER 330, 60 + i
MAUER 330, 240 + i
MAUER 330 + i, 210
MAUER 420 + i, 210
NEXT i
MAUER 210, 180
MAUER 240, 150
MAUER 270, 120
MAUER 300, 90
MAUER 210, 240
MAUER 240, 270
MAUER 270, 300
MAUER 300, 330
FELS 330, 240
DING 16, 60, 150, 0
DING 16, 60, 270, 0
DING 9, 90, 150, 0
DING 9, 90, 270, 0
DING 14, 240, 210, 13
DING 1, 510, 30, 0
DING 1, 540, 60, 0
DING 1, 570, 90, 0
DING 1, 540, 120, 0
DING 1, 510, 150, 0
DING 1, 480, 120, 0
DING 1, 450, 90, 0
DING 1, 480, 60, 0
DING 1, 510, 30 + 240, 0
DING 1, 540, 60 + 240, 0
DING 1, 570, 90 + 240, 0
DING 1, 540, 120 + 240, 0
DING 1, 510, 150 + 240, 0
DING 1, 480, 120 + 240, 0
DING 1, 450, 90 + 240, 0
DING 1, 480, 60 + 240, 0
TOR 510, 330, 13
OPEN "timer21.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i)
INPUT #1, viehy(i)
INPUT #1, viehr(i)
MONSTER viehx(i), viehy(i), 1, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 510: y = 90: r = 1
CASE 22
FOR i = 0 TO 90 STEP 30
MAUER 150, 180 + i
MAUER 180 + i, 150
MAUER 300 + i, 150
MAUER 30 + i, 210
MAUER 150 + i, 270
MAUER 270 + i, 270
MAUER 480 + i, 210
MAUER 450, 180 + i
MAUER 360 + i, 270
MAUER 360 + i, 150
DING 2, 30 + i, 30, 0
DING 1, 30 + i, 60, 0
DING 16, 30 + i, 90, 0
DING 1, 30 + i, 120, 0
DING 17, 30 + i, 150, 0
DING 1, 30 + i, 180, 0
DING 16, 150, 30 + i, 0
DING 1, 180, 30 + i, 0
NEXT i
BLACK 300, 150, 4
BLACK 300, 270, 4
DING 1, 300, 150, 0
DING 1, 300, 270, 0
MAUER 150, 150
TOR 300, 360, 9
DING 14, 510, 270, 9
DING 9, 510, 300, 0
OPEN "timer22.da1" FOR INPUT AS #1
FOR i = 1 TO 27
INPUT #1, viehx(i)
INPUT #1, viehy(i)
INPUT #1, viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 510: y = 150: r = 3
CASE 23
FOR i = 0 TO 60 STEP 30
DING 1, 180, i + 300, 0
DING 16, 30, i + 30, 0
DING 16, 30, i + 120, 0
DING 16, 60, i + 30, 0
DING 16, 60, i + 120, 0
DING 16, 90, i + 30, 0
DING 16, 90, i + 120, 0
DING 16, 120, i + 30, 0
DING 16, 120, i + 120, 0
DING 16, 150, i + 30, 0
DING 16, 150, i + 120, 0
BLACK 30 + i, 180, 4
NEXT i
BLACK 30, 180, 4
BLACK 60, 180, 4
BLACK 90, 180, 4
BLACK 120, 120, 4
DING 1, 30, 180, 0
DING 1, 60, 180, 0
DING 1, 90, 180, 0
FOR i = 0 TO 150 STEP 30
MAUER i, 210
MAUER 60 + i, 210
MAUER 240, 90 + i
MAUER 270 + i, 180 - i
MAUER 270 + i, 240 + i
MAUER 240, 330 - i
DING 1, 270 + i, 210, 0
DING 1, 570 - i, 210, 0
NEXT i
MAUER 60, 60
MAUER 90, 60
MAUER 120, 60
MAUER 90, 90
MAUER 90, 120
MAUER 150, 120
MAUER 150, 150
DING 14, 330, 330, 5
TOR 300, 60, 5
DING 7, 390, 390, 11
BLACK 330, 120, 4
BLACK 390, 60, 4
BLACK 330, 300, 4
BLACK 390, 360, 4
BLACK 270, 180, 4
BLACK 270, 240, 4
DING 3, 120, 120, 0
OPEN "timer23.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i)
INPUT #1, viehy(i)
INPUT #1, viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 120: y = 330: r = 2
CASE 24
FOR i = 0 TO 60 STEP 30
MAUER 60 + i, 60
MAUER 60 + i, 120
MAUER 90, 150 + i
MAUER 150 + i, 180
MAUER 390, 30 + i
MAUER 330, 30 + i
MAUER 480 + i, 120
MAUER 390 + i, 300
MAUER 510 + i, 300
MAUER 420 + i, 240
MAUER 330 + i, 360
MAUER 450 + i, 360
NEXT i
FOR i = 0 TO 90 STEP 30
MAUER 180, 30 + i
MAUER 240 + i, 120
MAUER 450 + i, 60
MAUER 360 + i, 180
MAUER 510, 180 + i
MAUER 60, 270 + i
MAUER 120, 300 + i
MAUER 180, 270 + i
MAUER 240, 300 + i
MAUER 60 + i, 240
MAUER 300, 180 + i
MAUER 300, 270 + i
NEXT i
MAUER 180, 240
MAUER 570, 240
MAUER 360, 240
MAUER 480, 90
MAUER 570, 60
DING 14, 90, 30, 9
TOR 540, 240, 9
DING 14, 210, 30, 10
TOR 30, 390, 10
DING 1, 210, 60, 0
DING 1, 240, 60, 0
DING 1, 360, 60, 0
DING 14, 570, 270, 11
TOR 450, 30, 11
DING 14, 570, 30, 12
TOR 210, 120, 12
DING 1, 510, 30, 0
DING 1, 540, 90, 0
DING 16, 240, 180, 0
DING 16, 120, 180, 0
DING 16, 60, 180, 0
DING 16, 150, 60, 0
DING 16, 480, 180, 0
DING 1, 510, 330, 0
DING 1, 510, 390, 0
DING 17, 540, 330, 0
DING 16, 570, 330, 0
DING 17, 540, 360, 0
DING 7, 540, 390, 14
DING 3, 570, 360, 0
DING 1, 240, 90, 0
MAUER 30, 180
MAUER 420, 120
MAUER 540, 180
MAUER 210, 240
MAUER 240, 150
WASSER 330, 300
WASSER 240, 240
DING 1, 30, 300, 0
DING 1, 90, 330, 0
OPEN "timer24.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i)
INPUT #1, viehy(i)
INPUT #1, viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
x = 570: y = 390: r = 1
CASE 25
FOR i = 0 TO 180 STEP 30
MAUER 90 + i, 90
MAUER 330 + i, 90
MAUER 300, 30 + i
MAUER 300, 210 + i
MAUER 90, 120 + i
MAUER 90 + i, 330
MAUER 330 + i, 330
MAUER 510, 120 + i
NEXT i
MAUER 30, 210: MAUER 60, 210: MAUER 540, 210: MAUER 570, 210
MAUER 150, 60: MAUER 240, 60: MAUER 360, 60: MAUER 450, 60
MAUER 150, 360: MAUER 240, 360: MAUER 360, 360: MAUER 450, 360
MAUER 120, 210: MAUER 270, 210: MAUER 330, 210: MAUER 480, 210
SCHLEUSE 150, 60, 3: SCHLEUSE 240, 60, 4: SCHLEUSE 360, 60, 4: SCHLEUSE 450, 60, 3
SCHLEUSE 120, 210, 2: SCHLEUSE 270, 210, 2: SCHLEUSE 330, 210, 2: SCHLEUSE 480, 210, 2
SCHLEUSE 150, 360, 3: SCHLEUSE 240, 360, 4: SCHLEUSE 360, 360, 4: SCHLEUSE 450, 360, 3
feld(15, 6) = 100: feld(24, 6) = 1: feld(36, 6) = 1: feld(45, 6) = 100
feld(12, 21) = 1: feld(27, 21) = 100: feld(33, 21) = 1: feld(48, 21) = 100
feld(15, 36) = 1: feld(24, 36) = 100: feld(36, 36) = 100: feld(45, 36) = 1
DING 14, 60, 180, 1
TOR 60, 240, 1
DING 1, 120, 30, 0
DING 3, 90, 390, 0
DING 1, 90, 360, 0
OPEN "timer25.da1" FOR INPUT AS #1
FOR i = 1 TO 12
INPUT #1, viehx(i), viehy(i), viehr(i)
MONSTER viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 570: y = 390: r = 1
CASE ELSE
END SELECT
END SUB

SUB MAUER (x, y)

feld(x / 10, y / 10) = 1
COLOR 8
LINE (x, y)-(x + 30, y + 30), 8, B
PAINT (x + 12, y + 12), 8
LINE (x + 2, y + 2)-(x + 28, y + 28), 7, B
LINE (x + 3, y + 3)-(x + 27, y + 27), 7, B

END SUB

SUB MONSTER (x, y, z, a)

IF z = 1 THEN GOTO skelett
IF z = 2 THEN GOTO vogel
IF z = 3 THEN GOTO killer
'-----------------------------------------------------------------------------
skelett:
IF a = 1 THEN GOTO ske1
IF a = 2 THEN GOTO ske2
IF a = 3 THEN GOTO ske3
IF a = 4 THEN GOTO ske4

ske1:
COLOR 15
CIRCLE (x + 15, y + 10), 9
PAINT (x + 15, y + 10)
LINE (x + 10, y + 10)-(x + 20, y + 22), , BF
LINE (x + 11, y + 23)-(x + 19, y + 23), , BF
COLOR 0
LINE (x + 10, y + 15)-(x + 12, y + 15)
LINE (x + 11, y + 14)-(x + 11, y + 16)
LINE (x + 20, y + 15)-(x + 18, y + 15)
LINE (x + 19, y + 14)-(x + 19, y + 16)
LINE (x + 10, y + 15)-(x + 12, y + 15)
LINE (x + 11, y + 14)-(x + 11, y + 16)
LINE (x + 16, y + 15)-(x + 15, y + 18), , BF
LINE (x + 17, y + 18)-(x + 14, y + 18)
LINE (x + 17, y + 19)-(x + 14, y + 19)
LINE (x + 19, y + 21)-(x + 12, y + 21)
GOTO 10

ske2:
COLOR 15
CIRCLE (x + 15, y + 20), 9
PAINT (x + 15, y + 20)
LINE (x + 10, y + 20)-(x + 20, y + 8), , BF
LINE (x + 11, y + 7)-(x + 19, y + 7), , BF
COLOR 0
LINE (x + 10, y + 15)-(x + 12, y + 15)
LINE (x + 11, y + 16)-(x + 11, y + 14)
LINE (x + 20, y + 15)-(x + 18, y + 15)
LINE (x + 19, y + 16)-(x + 19, y + 14)
LINE (x + 14, y + 15)-(x + 15, y + 12), , BF
LINE (x + 13, y + 12)-(x + 16, y + 12)
LINE (x + 13, y + 11)-(x + 16, y + 11)
LINE (x + 11, y + 9)-(x + 18, y + 9)
GOTO 10

ske3:
COLOR 15
CIRCLE (x + 10, y + 15), 9
PAINT (x + 10, y + 15)
LINE (x + 10, y + 10)-(x + 22, y + 20), , BF
LINE (x + 23, y + 11)-(x + 23, y + 19), , BF
COLOR 0
LINE (x + 15, y + 10)-(x + 15, y + 12)
LINE (x + 14, y + 11)-(x + 16, y + 11)
LINE (x + 15, y + 20)-(x + 15, y + 18)
LINE (x + 14, y + 19)-(x + 16, y + 19)
LINE (x + 15, y + 14)-(x + 18, y + 15), , BF
LINE (x + 18, y + 13)-(x + 18, y + 16)
LINE (x + 19, y + 13)-(x + 19, y + 16)
LINE (x + 21, y + 11)-(x + 21, y + 18)
GOTO 10

ske4:
COLOR 15
CIRCLE (x + 20, y + 15), 9
PAINT (x + 20, y + 15)
LINE (x + 20, y + 10)-(x + 8, y + 20), , BF
LINE (x + 7, y + 11)-(x + 7, y + 19), , BF
COLOR 0
LINE (x + 15, y + 10)-(x + 15, y + 12)
LINE (x + 16, y + 11)-(x + 14, y + 11)
LINE (x + 15, y + 20)-(x + 15, y + 18)
LINE (x + 16, y + 19)-(x + 14, y + 19)
LINE (x + 15, y + 14)-(x + 12, y + 15), , BF
LINE (x + 12, y + 13)-(x + 12, y + 16)
LINE (x + 11, y + 13)-(x + 11, y + 16)
LINE (x + 9, y + 11)-(x + 9, y + 18)
GOTO 10
'-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
vogel:
IF a = 1 THEN GOTO vog1
IF a = 2 THEN GOTO vog2
IF a = 3 THEN GOTO vog3
IF a = 4 THEN GOTO vog4

vog1:
COLOR 14
CIRCLE (x + 15, y + 10), 9
PAINT (x + 15, y + 10)
COLOR 6
LINE (x + 12, y + 19)-(x + 15, y + 24)
LINE (x + 18, y + 19)-(x + 15, y + 24)
LINE (x + 12, y + 19)-(x + 18, y + 19)
PAINT (x + 15, y + 20)
COLOR 0
LINE (x + 11, y + 15)-(x + 13, y + 15)
LINE (x + 19, y + 15)-(x + 17, y + 15)
LINE (x + 10, y + 14)-(x + 12, y + 14)
LINE (x + 18, y + 14)-(x + 20, y + 14)
GOTO 10

vog2:
COLOR 14
CIRCLE (x + 15, y + 20), 9
PAINT (x + 15, y + 20)
COLOR 6
LINE (x + 12, y + 11)-(x + 15, y + 6)
LINE (x + 18, y + 11)-(x + 15, y + 6)
LINE (x + 12, y + 11)-(x + 18, y + 11)
PAINT (x + 15, y + 10)
COLOR 0
LINE (x + 11, y + 15)-(x + 13, y + 15)
LINE (x + 19, y + 15)-(x + 17, y + 15)
LINE (x + 10, y + 16)-(x + 12, y + 16)
LINE (x + 18, y + 16)-(x + 20, y + 16)
GOTO 10

vog3:
COLOR 14
CIRCLE (x + 10, y + 15), 9
PAINT (x + 10, y + 15)
COLOR 6
LINE (x + 19, y + 12)-(x + 24, y + 15)
LINE (x + 19, y + 18)-(x + 24, y + 15)
LINE (x + 19, y + 12)-(x + 19, y + 18)
PAINT (x + 20, y + 15)
COLOR 0
LINE (x + 15, y + 11)-(x + 15, y + 13)
LINE (x + 15, y + 19)-(x + 15, y + 17)
LINE (x + 14, y + 10)-(x + 14, y + 12)
LINE (x + 14, y + 18)-(x + 14, y + 20)
GOTO 10

vog4:
COLOR 14
CIRCLE (x + 20, y + 15), 9
PAINT (x + 20, y + 15)
COLOR 6
LINE (x + 11, y + 12)-(x + 6, y + 15)
LINE (x + 11, y + 18)-(x + 6, y + 15)
LINE (x + 11, y + 12)-(x + 11, y + 18)
PAINT (x + 10, y + 15)
COLOR 0
LINE (x + 15, y + 11)-(x + 15, y + 13)
LINE (x + 15, y + 19)-(x + 15, y + 17)
LINE (x + 16, y + 10)-(x + 16, y + 12)
LINE (x + 16, y + 18)-(x + 16, y + 20)
GOTO 10
'-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-
killer:
IF a = 1 THEN GOTO kill1
IF a = 2 THEN GOTO kill2
IF a = 3 THEN GOTO kill3
IF a = 4 THEN GOTO kill4

kill1:
LINE (x + 7, y + 15)-(x + 10, y + 30), 2, BF
LINE (x + 24, y + 15)-(x + 21, y + 30), 2, BF
CIRCLE (x + 15, y + 20), 8, 1
PAINT STEP(0, 0), 1
CIRCLE (x + 16, y + 20), 8, 1
COLOR 7
CIRCLE (x + 15, y + 20), 6
PAINT STEP(0, 0)
LINE (x + 11, y + 18)-(x + 11, y + 7)
LINE (x + 9, y + 20)-(x + 9, y + 9)
LINE (x + 13, y + 18)-(x + 13, y + 9)
LINE (x + 15, y + 18)-(x + 15, y + 7)
LINE (x + 17, y + 18)-(x + 17, y + 9)
LINE (x + 19, y + 18)-(x + 19, y + 7)
LINE (x + 21, y + 20)-(x + 21, y + 9)
COLOR 4
LINE (x + 12, y + 23)-(x + 13, y + 23)
LINE (x + 18, y + 23)-(x + 17, y + 23)
LINE (x + 12, y + 22)-(x + 13, y + 24)
LINE (x + 18, y + 22)-(x + 17, y + 24)
GOTO 10

kill2:
LINE (x + 7, y + 15)-(x + 10, y + 0), 2, BF
LINE (x + 24, y + 15)-(x + 21, y + 0), 2, BF
CIRCLE (x + 15, y + 10), 8, 1
PAINT STEP(0, 0), 1
CIRCLE (x + 16, y + 10), 8, 1
COLOR 7
CIRCLE (x + 15, y + 10), 6
PAINT STEP(0, 0)
LINE (x + 11, y + 12)-(x + 11, y + 23)
LINE (x + 9, y + 10)-(x + 9, y + 21)
LINE (x + 13, y + 12)-(x + 13, y + 21)
LINE (x + 15, y + 12)-(x + 15, y + 23)
LINE (x + 17, y + 12)-(x + 17, y + 21)
LINE (x + 19, y + 12)-(x + 19, y + 23)
LINE (x + 21, y + 10)-(x + 21, y + 21)
COLOR 4
LINE (x + 12, y + 7)-(x + 13, y + 7)
LINE (x + 18, y + 7)-(x + 17, y + 7)
LINE (x + 12, y + 8)-(x + 13, y + 6)
LINE (x + 18, y + 8)-(x + 17, y + 6)
GOTO 10

kill3:
LINE (x + 15, y + 7)-(x + 30, y + 10), 2, BF
LINE (x + 15, y + 24)-(x + 30, y + 21), 2, BF
CIRCLE (x + 20, y + 15), 8, 1
PAINT STEP(0, 0), 1
CIRCLE (x + 20, y + 16), 8, 1
COLOR 7
CIRCLE (x + 20, y + 15), 6
PAINT STEP(0, 0)
LINE (x + 18, y + 11)-(x + 7, y + 11)
LINE (x + 20, y + 9)-(x + 9, y + 9)
LINE (x + 18, y + 13)-(x + 9, y + 13)
LINE (x + 15, y + 15)-(x + 7, y + 15)
LINE (x + 18, y + 17)-(x + 9, y + 17)
LINE (x + 18, y + 19)-(x + 7, y + 19)
LINE (x + 20, y + 21)-(x + 9, y + 21)
COLOR 4
LINE (x + 23, y + 12)-(x + 23, y + 13)
LINE (x + 23, y + 18)-(x + 23, y + 17)
LINE (x + 22, y + 12)-(x + 24, y + 13)
LINE (x + 22, y + 18)-(x + 24, y + 17)
GOTO 10

kill4:
LINE (x + 15, y + 7)-(x + 0, y + 10), 2, BF
LINE (x + 15, y + 24)-(x + 0, y + 21), 2, BF
CIRCLE (x + 10, y + 15), 8, 1
PAINT STEP(0, 0), 1
CIRCLE (x + 10, y + 16), 8, 1
COLOR 7
CIRCLE (x + 10, y + 15), 6
PAINT STEP(0, 0)
LINE (x + 12, y + 11)-(x + 23, y + 11)
LINE (x + 10, y + 9)-(x + 21, y + 9)
LINE (x + 12, y + 13)-(x + 21, y + 13)
LINE (x + 15, y + 15)-(x + 23, y + 15)
LINE (x + 12, y + 17)-(x + 21, y + 17)
LINE (x + 12, y + 19)-(x + 23, y + 19)
LINE (x + 10, y + 21)-(x + 21, y + 21)
COLOR 4
LINE (x + 7, y + 12)-(x + 7, y + 13)
LINE (x + 7, y + 18)-(x + 7, y + 17)
LINE (x + 8, y + 12)-(x + 6, y + 13)
LINE (x + 8, y + 18)-(x + 6, y + 17)
GOTO 10

10 END SUB

SUB NEPOMUK

CIRCLE (8, 15), 8, 15
CIRCLE (10, 12), 2, 3
CIRCLE (10, 18), 2, 3
PSET (11, 12), 15
PSET (11, 18), 15
CIRCLE (25, 15), 3, 15
PSET (21, 15), 0
PSET (21, 16), 0
PAINT (25, 15), 0
LINE (24, 12)-(18, 12), 15
LINE (24, 18)-(18, 18), 15
PSET (17, 19), 15
PSET (17, 11), 15
PSET (16, 20), 15
PSET (16, 10), 15
PSET (15, 21), 15
PSET (15, 9), 15
PSET (22, 14), 15
PSET (22, 16), 15
PSET (23, 14), 15
PSET (23, 16), 15
PSET (24, 13), 15
PSET (24, 17), 15
PAINT (18, 15), 15
PSET (8, 6), 0
PSET (7, 6), 15
PSET (9, 6), 15
PSET (7, 24), 15
PSET (8, 24), 0
PSET (9, 24), 15
LINE (6, 5)-(10, 5), 15
LINE (6, 25)-(10, 25), 15
COLOR 13
PSET (5, 15)
PSET (4, 14)
PSET (3, 13)
PSET (4, 16)
PSET (3, 17)
LINE (3, 15)-(7, 15)
GET (0, 0)-(30, 30), nepomuk2
LINE (0, 0)-(30, 30), 0, BF
CIRCLE (22, 15), 8, 15
CIRCLE (20, 12), 2, 3
CIRCLE (20, 18), 2, 3
PSET (19, 12), 15
PSET (19, 18), 15
CIRCLE (5, 15), 3, 15
PSET (9, 15), 0
PSET (9, 16), 0
PAINT (5, 15), 0
LINE (6, 12)-(12, 12), 15
LINE (6, 18)-(12, 18), 15
PSET (13, 19), 15
PSET (13, 11), 15
PSET (14, 20), 15
PSET (14, 10), 15
PSET (15, 21), 15
PSET (15, 9), 15
PSET (8, 14), 15
PSET (8, 16), 15
PSET (7, 14), 15
PSET (7, 16), 15
PSET (6, 13), 15
PSET (6, 17), 15
PAINT (12, 15), 15
PSET (22, 6), 0
PSET (23, 6), 15
PSET (21, 6), 15
PSET (23, 24), 15
PSET (22, 24), 0
PSET (21, 24), 15
LINE (24, 5)-(20, 5), 15
LINE (24, 25)-(20, 25), 15
COLOR 13
PSET (25, 15)
PSET (26, 14)
PSET (27, 13)
PSET (26, 16)
PSET (27, 17)
LINE (27, 15)-(23, 15)
GET (0, 0)-(30, 30), nepomuk1
LINE (0, 0)-(30, 30), 0, BF
CIRCLE (15, 8), 8, 15
CIRCLE (12, 10), 2, 3
CIRCLE (18, 10), 2, 3
PSET (12, 11), 15
PSET (18, 11), 15
CIRCLE (15, 25), 3, 15
PSET (15, 21), 0
PSET (16, 21), 0
PAINT (15, 25), 0
LINE (12, 24)-(12, 18), 15
LINE (18, 24)-(18, 18), 15
PSET (19, 17), 15
PSET (11, 17), 15
PSET (20, 16), 15
PSET (10, 16), 15
PSET (21, 15), 15
PSET (9, 15), 15
PSET (14, 21), 15
PSET (16, 22), 15
PSET (14, 23), 15
PSET (16, 23), 15
PSET (13, 24), 15
PSET (17, 24), 15
PAINT (15, 18), 15
PSET (6, 8), 0
PSET (6, 7), 15
PSET (6, 9), 15
PSET (24, 7), 15
PSET (24, 8), 0
PSET (24, 9), 15
LINE (5, 6)-(5, 10), 15
LINE (25, 6)-(25, 10), 15
COLOR 13
PSET (15, 5)
PSET (14, 4)
PSET (13, 3)
PSET (16, 4)
PSET (17, 3)
LINE (15, 3)-(15, 7)
GET (0, 0)-(30, 30), nepomuk3
LINE (0, 0)-(30, 30), 0, BF
CIRCLE (15, 22), 8, 15
CIRCLE (12, 20), 2, 3
CIRCLE (18, 20), 2, 3
PSET (12, 19), 15
PSET (18, 19), 15
CIRCLE (15, 5), 3, 15
PSET (15, 9), 0
PSET (16, 9), 0
PAINT (15, 5), 0
LINE (12, 6)-(12, 12), 15
LINE (18, 6)-(18, 12), 15
PSET (19, 13), 15
PSET (11, 13), 15
PSET (20, 14), 15
PSET (10, 14), 15
PSET (21, 15), 15
PSET (9, 15), 15
PSET (14, 9), 15
PSET (16, 9), 15
PSET (14, 7), 15
PSET (16, 7), 15
PSET (13, 6), 15
PSET (17, 6), 15
PAINT (15, 12), 15
PSET (6, 22), 0
PSET (6, 23), 15
PSET (6, 21), 15
PSET (24, 23), 15
PSET (24, 22), 0
PSET (24, 21), 15
LINE (5, 24)-(5, 20), 15
LINE (25, 24)-(25, 20), 15
COLOR 13
PSET (15, 25)
PSET (14, 26)
PSET (13, 27)
PSET (16, 26)
PSET (17, 27)
LINE (15, 27)-(15, 23)
GET (0, 0)-(30, 30), nepomuk4
BLACK 0, 0, 0
CIRCLE (15, 15), 3, 0
PAINT STEP(0, 0), 3, 0
GET (1, 1)-(29, 29), skern
LINE (0, 0)-(30, 30), 0, BF

END SUB

SUB SCHLEUSE (x, y, z)

IF z = 1 THEN GOSUB Schleuse1: feld(x / 10, y / 10) = 32
IF z = 2 THEN GOSUB Schleuse2: feld(x / 10, y / 10) = 31
IF z = 3 THEN GOSUB Schleuse3: feld(x / 10, y / 10) = 34
IF z = 4 THEN GOSUB Schleuse4: feld(x / 10, y / 10) = 33

Schleuse1:
COLOR 12
LINE (x + 7, y + 15)-(x + 14, y + 8)
LINE (x + 14, y + 8)-(x + 14, y + 23)
LINE (x + 7, y + 16)-(x + 14, y + 23)
LINE (x + 23, y + 18)-(x + 14, y + 12), , B
PAINT (x + 8, y + 15)
PAINT (x + 15, y + 15)
EXIT SUB

Schleuse2:
COLOR 12
LINE (x + 23, y + 15)-(x + 16, y + 8)
LINE (x + 16, y + 8)-(x + 16, y + 23)
LINE (x + 23, y + 16)-(x + 16, y + 23)
LINE (x + 7, y + 18)-(x + 16, y + 12), , B
PAINT (x + 22, y + 15)
PAINT (x + 15, y + 15)
EXIT SUB

Schleuse3:
COLOR 12
LINE (x + 15, y + 23)-(x + 8, y + 16)
LINE (x + 8, y + 16)-(x + 23, y + 16)
LINE (x + 16, y + 23)-(x + 23, y + 16)
LINE (x + 18, y + 7)-(x + 12, y + 16), , B
PAINT (x + 15, y + 22)
PAINT (x + 15, y + 15)
EXIT SUB

Schleuse4:
COLOR 12
LINE (x + 15, y + 7)-(x + 8, y + 14)
LINE (x + 8, y + 14)-(x + 23, y + 14)
LINE (x + 16, y + 7)-(x + 23, y + 14)
LINE (x + 18, y + 23)-(x + 12, y + 14), , B
PAINT (x + 15, y + 8)
PAINT (x + 15, y + 15)

END SUB

SUB TIMERS (level%, ZUSATZ)
IF ZUSATZ = 1 THEN GOTO sluss
IF ZUSATZ = 2 THEN GOTO trnk
SELECT CASE level%
CASE 3
FOR a = 30 TO 570 STEP 30
IF EOF(1) <> 0 THEN SEEK #1, 1
INPUT #1, dat
IF dat = 0 THEN BLACK a, 150, 4
IF dat = 1 THEN MAUER a, 150
NEXT a
FOR a = 30 TO 570 STEP 30
IF EOF(2) <> 0 THEN SEEK #2, 1
INPUT #2, dat1
IF dat1 = 0 THEN BLACK a, 210, 4
IF dat1 = 1 THEN MAUER a, 210
NEXT a
IF y = 210 THEN x = x + 30: PUT (x, y), nepomuk2
IF y = 150 THEN x = x - 30: PUT (x, y), nepomuk1
CASE 7
BLACK wwasser(1), 60, 4: feld(wwasser(1) / 10, 6) = 0
BLACK 270, wwasser(2), 4: feld(27, wwasser(2) / 10) = 0
BLACK 360, wwasser(3), 4: feld(36, wwasser(3) / 10) = 0
BLACK wwasser(4), 240, 4: feld(wwasser(4) / 10, 24) = 0
BLACK wwasser(4), 270, 4: feld(wwasser(4) / 10, 27) = 0
BLACK wwasser(5), 240, 4: feld(wwasser(5) / 10, 24) = 0
BLACK wwasser(6), 150, 4: feld(wwasser(6) / 10, 15) = 0
BLACK wwasser(7), 150, 4: feld(wwasser(7) / 10, 15) = 0
IF wwasser(1) = 300 THEN wwasser(1) = 330 ELSE wwasser(1) = 300
IF wwasser(2) = 180 THEN wwasser(2) = 210 ELSE wwasser(2) = 180
IF wwasser(3) = 180 THEN wwasser(3) = 210 ELSE wwasser(3) = 180
IF wwasser(4) = 90 THEN wwasser(4) = 120 ELSE wwasser(4) = 90
IF wwasser(5) = 480 THEN wwasser(5) = 510 ELSE wwasser(5) = 480
IF wwasser(6) = 480 THEN wwasser(6) = 510 ELSE wwasser(6) = 480
IF wwasser(7) = 90 THEN wwasser(7) = 120 ELSE wwasser(7) = 90
WASSER wwasser(1), 60: feld(wwasser(1) / 10, 6) = 27
WASSER 270, wwasser(2): feld(27, wwasser(2) / 10) = 27
WASSER 360, wwasser(3): feld(36, wwasser(3) / 10) = 27
WASSER wwasser(4), 240: feld(wwasser(4) / 10, 24) = 27
WASSER wwasser(4), 270: feld(wwasser(4) / 10, 27) = 27
WASSER wwasser(5), 240: feld(wwasser(5) / 10, 24) = 27
WASSER wwasser(6), 150: feld(wwasser(6) / 10, 15) = 27
WASSER wwasser(7), 150: feld(wwasser(7) / 10, 15) = 27
IF feld(x / 10, y / 10) = 27 THEN GOSUB wassertod2
CASE 8:
BLACK 90, wwasser(1), 4: feld(9, wwasser(1) / 10) = 0
BLACK 150, wwasser(2), 4: feld(15, wwasser(2) / 10) = 0
BLACK 210, wwasser(3), 4: feld(21, wwasser(3) / 10) = 0
BLACK 540, wwasser(4), 4: feld(54, wwasser(4) / 10) = 0
IF wwasser(1) = 300 THEN wwasser(1) = 240 ELSE wwasser(1) = 300
IF wwasser(2) = 330 THEN wwasser(2) = 390 ELSE wwasser(2) = 330
IF wwasser(3) = 330 THEN wwasser(3) = 270 ELSE wwasser(3) = 330
IF wwasser(4) = 390 THEN wwasser(4) = 240 ELSE wwasser(4) = 390
WASSER 90, wwasser(1): feld(9, wwasser(1) / 10) = 27
WASSER 150, wwasser(2): feld(15, wwasser(2) / 10) = 27
WASSER 210, wwasser(3): feld(21, wwasser(3) / 10) = 27
WASSER 540, wwasser(4): feld(54, wwasser(4) / 10) = 27
IF feld(x / 10, y / 10) = 27 THEN GOSUB wassertod2
CASE 9
FOR l9l = 60 TO 540 STEP 120
BLACK l9l, wwasser(1), 4: feld(l9l / 10, wwasser(1) / 10) = 0
BLACK l9l, wwasser(1) + 30, 4: feld(l9l / 10, wwasser(1) / 10 + 3) = 0
NEXT l9l
FOR l9l = 120 TO 480 STEP 120
BLACK l9l, wwasser(2), 4: feld(l9l / 10, wwasser(2) / 10) = 0
BLACK l9l, wwasser(2) + 30, 4: feld(l9l / 10, wwasser(2) / 10 + 3) = 0
NEXT l9l
IF wwasser(1) = 30 THEN wwasser(1) = 360 ELSE wwasser(1) = 30
IF wwasser(2) = 30 THEN wwasser(2) = 360 ELSE wwasser(2) = 30
FOR l9l = 60 TO 540 STEP 120
WASSER l9l, wwasser(1): feld(l9l / 10, wwasser(1) / 10) = 27
WASSER l9l, wwasser(1) + 30: feld(l9l / 10, wwasser(1) / 10 + 3) = 27
NEXT l9l
FOR l9l = 120 TO 480 STEP 120
WASSER l9l, wwasser(2): feld(l9l / 10, wwasser(2) / 10) = 27
WASSER l9l, wwasser(2) + 30: feld(l9l / 10, wwasser(2) / 10 + 3) = 27
NEXT l9l
IF feld(x / 10, y / 10) = 27 THEN GOSUB wassertod2
CASE 12
FOR i = 1 TO 14
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 3: kern viehx(i), viehy(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 3: kern viehx(i), viehy(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 3: kern viehx(i), viehy(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 3: kern viehx(i), viehy(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 14
FOR i = 1 TO 4
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 15
IF wmauer(1) = 0 THEN wmauer(1) = 1 ELSE wmauer(1) = 0
IF wmauer(3) = 0 THEN wmauer(3) = 1 ELSE wmauer(3) = 0
IF wmauer(1) = 1 THEN BLACK 390, 270, 8: MAUER 390, 330
IF wmauer(1) = 0 THEN BLACK 390, 330, 2: MAUER 390, 270
IF EOF(1) <> 0 THEN SEEK #1, 1
INPUT #1, wmauer(2)
IF wmauer(2) = 1 THEN BLACK 420, 330, 4: BLACK 420, 240, 4: MAUER 420, 300: MAUER 420, 270
IF wmauer(2) = 0 THEN BLACK 420, 330, 2: BLACK 420, 300, 4: MAUER 420, 270: MAUER 420, 240
IF wmauer(2) = 2 THEN BLACK 420, 240, 4: BLACK 420, 270, 4: MAUER 420, 300: MAUER 420, 330
IF wmauer(3) = 1 THEN BLACK 540, 270, 4: MAUER 540, 330
IF wmauer(3) = 0 THEN BLACK 540, 330, 2: MAUER 540, 270
IF x = 390 AND y = 330 AND wmauer(1) = 1 THEN GOSUB wassertod2
IF x = 420 AND y = 330 AND wmauer(2) = 2 THEN GOSUB wassertod2
IF x = 540 AND y = 330 AND wmauer(3) = 1 THEN GOSUB wassertod2
IF x = 390 AND y = 270 AND wmauer(1) = 0 THEN y = y - 30: r = 4: PUT (x, y), nepomuk4
IF x = 420 AND y = 270 AND wmauer(2) = 1 THEN y = y - 30: r = 4: PUT (x, y), nepomuk4
IF x = 420 AND y = 240 AND wmauer(2) = 0 THEN y = y - 30: r = 4: PUT (x, y), nepomuk4
IF x = 540 AND y = 270 AND wmauer(3) = 0 THEN y = y - 30: r = 4: PUT (x, y), nepomuk4
FOR i = 1 TO 14
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 16
IF EOF(1) <> 0 THEN SEEK #1, 1
IF EOF(2) <> 0 THEN SEEK #2, 1
IF EOF(3) <> 0 THEN SEEK #3, 1
BLACK wwasser(1), 150, 4: BLACK wwasser(1) + 30, 150, 4: BLACK wwasser(1) + 8 * 30, 150, 4: BLACK wwasser(1) + 7 * 30, 150, 4
INPUT #1, wwasser(1)
WASSER wwasser(1), 150: WASSER wwasser(1) + 30, 150: WASSER wwasser(1) + 7 * 30, 150: WASSER wwasser(1) + 8 * 30, 150
BLACK wwasser(2), 210, 4: BLACK wwasser(2) + 30, 210, 4: BLACK wwasser(2) + 8 * 30, 210, 4: BLACK wwasser(2) + 7 * 30, 210, 4
INPUT #1, wwasser(2)
WASSER wwasser(2), 210: WASSER wwasser(2) + 30, 210: WASSER wwasser(2) + 7 * 30, 210: WASSER wwasser(2) + 8 * 30, 210
BLACK wwasser(3), 270, 4: BLACK wwasser(3) + 30, 270, 4: BLACK wwasser(3) + 8 * 30, 270, 4: BLACK wwasser(3) + 7 * 30, 270, 4
INPUT #1, wwasser(3)
WASSER wwasser(3), 270: WASSER wwasser(3) + 30, 270: WASSER wwasser(3) + 7 * 30, 270: WASSER wwasser(3) + 8 * 30, 270
BLACK wwasser(7), 90, 4
INPUT #2, wwasser(7)
WASSER wwasser(7), 90
BLACK wwasser(8), 330, 4
INPUT #3, wwasser(8)
WASSER wwasser(8), 330
IF feld(x / 10, y / 10) = 27 THEN GOSUB wassertod2
FOR i = 1 TO 10
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 17
FOR i = 1 TO 8
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
FOR i = 1 TO 20
BLACK wwasser(i), wwassery(i), 4
NEXT i
FOR i = 1 TO 20
IF EOF(1) <> 0 THEN SEEK #1, 1
INPUT #1, wwasser(i), wwassery(i)
IF EOF(1) <> 0 THEN SEEK #1, 1
WASSER wwasser(i), wwassery(i)
NEXT i
IF feld(x / 10, y / 10) = 27 THEN GOSUB wassertod2
CASE 18
FOR i = 1 TO 10
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 19
FOR i = 1 TO 29
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 20
FOR i = 1 TO 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 21
IF wwasser(1) = 0 THEN wwasser(1) = 1 ELSE wwasser(1) = 0
IF wwasser(1) = 0 THEN BLACK 570, 210, 4: BLACK 330, 390, 4: BLACK 150, 30, 4: WASSER 30, 210: WASSER 150, 390: WASSER 330, 30
IF wwasser(1) = 1 THEN WASSER 570, 210: WASSER 330, 390: WASSER 150, 30: BLACK 30, 210, 4: BLACK 150, 390, 4: BLACK 330, 30, 4
IF feld(x / 10, y / 10) = 27 THEN GOSUB wassertod2
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 OR feld(viehx(i) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 OR feld(viehx(i) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 OR feld(viehx(i) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 OR feld(viehx(i) / 10, viehy(i) / 10) = 27 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 22
FOR i = 1 TO 27
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 23
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) + 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 OR feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 27 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 OR feld((viehx(i) - 30) / 10, viehy(i) / 10) = 27 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 OR feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 27 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 24
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE 25
FOR i = 1 TO 12
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: BLACK viehx(i) - 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: BLACK viehx(i), viehy(i) - 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: BLACK viehx(i) + 30, viehy(i), 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: BLACK viehx(i), viehy(i) + 30, 4: MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
BLACK a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
CASE ELSE
END SELECT
EXIT SUB
sluss:
SELECT CASE level%
CASE 3
IF x = 30 AND y = 390 THEN schlussel(1) = 1
IF x = 330 AND y = 30 THEN schlussel(3) = 1
IF x = 480 AND y = 90 THEN schlussel(4) = 1
IF x = 30 AND y = 270 THEN schlussel(5) = 1
CASE 6
IF x = 30 AND y = 390 THEN schlussel(1) = 1
IF x = 90 AND y = 390 THEN schlussel(2) = 1
IF x = 150 AND y = 390 THEN schlussel(3) = 1
IF x = 210 AND y = 390 THEN schlussel(4) = 1
IF x = 270 AND y = 390 THEN schlussel(5) = 1
IF x = 330 AND y = 390 THEN schlussel(6) = 1
IF x = 390 AND y = 390 THEN schlussel(7) = 1
IF x = 450 AND y = 390 THEN schlussel(8) = 1
CASE 7
IF x = 30 AND y = 120 THEN schlussel(1) = 1
IF x = 30 AND y = 210 THEN schlussel(2) = 1
IF x = 390 AND y = 120 THEN schlussel(3) = 1
IF x = 570 AND y = 30 THEN schlussel(4) = 1
IF x = 390 AND y = 390 THEN schlussel(5) = 1
IF x = 570 AND y = 210 THEN schlussel(6) = 1
CASE 8
IF x = 570 AND y = 240 THEN schlussel(1) = 1
IF x = 570 AND y = 180 THEN schlussel(2) = 1
IF x = 330 AND y = 30 THEN schlussel(3) = 1
IF x = 30 AND y = 240 THEN schlussel(4) = 1
CASE 14
IF x = 60 AND y = 60 THEN schlussel(1) = 1
IF x = 540 AND y = 60 THEN schlussel(2) = 1
IF x = 60 AND y = 360 THEN schlussel(3) = 1
IF x = 540 AND y = 360 THEN schlussel(4) = 1
CASE 15:
IF x = 120 AND y = 90 THEN schlussel(1) = 1
IF x = 150 AND y = 90 THEN schlussel(2) = 1
CASE 20
IF x = 570 AND y = 30 THEN schlussel(1) = 1
IF x = 60 AND y = 90 THEN schlussel(2) = 1
IF x = 90 AND y = 390 THEN schlussel(3) = 1
CASE 24
IF x = 90 AND y = 30 THEN schlussel(1) = 1
IF x = 570 AND y = 270 THEN schlussel(2) = 1
IF x = 570 AND y = 30 THEN schlussel(3) = 1
IF x = 210 AND y = 30 THEN schlussel(4) = 1
CASE ELSE
END SELECT
EXIT SUB
wassertod2:
BEEP
RETURN
trnk:
SELECT CASE level
CASE 14
IF x = 240 AND y = 60 THEN
CLS
WINDOW (630, 480)-(0, 0)
IF feld(30, 6) = 0 AND feld(30, 30) <> 0 THEN
CLS
SELECT CASE r
CASE 1
PUT (x, y), nepomuk1
CASE 2
PUT (x, y), nepomuk2
CASE 3
PUT (x, y), nepomuk3
CASE 4
PUT (x, y), nepomuk4
END SELECT
IF feld(30, 30) <> 0 THEN DING 10, 300, 300, 1
IF lives > 28 THEN lives = 28
LINE (2, 457)-(123, 472), 5, B
FOR balken = 3 TO lives * 4
LINE (balken, 458)-(balken + 10, 471), 9, BF
NEXT balken
FOR leer = balken + 10 TO 118
LINE (leer, 458)-(leer + 4, 471), 0, BF
NEXT leer
GOTO weiter7
END IF
FOR a = 0 TO 600 STEP 30
MAUER a, 0
MAUER a, 420
NEXT
FOR a = 0 TO 420 STEP 30
MAUER 0, a
MAUER 600, a
NEXT
FOR a = 30 TO 570 STEP 30
FOR B = 30 TO 390 STEP 30
trankl14a = feld(a / 10, B / 10)
SELECT CASE trankl14a
CASE 1
MAUER a, B
CASE 2 TO 18
DING trankl14a - 1, a, B, 1
CASE 19
EIS a, B
CASE 21
TOR a, B, 1
CASE 25
FELS a, B
CASE ELSE
END SELECT
NEXT
NEXT
FOR i = 1 TO 4
IF feld(viehx(i) / 10, viehy(i) / 10) <> 0 THEN MONSTER viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT
IF schlussel(1) = 0 THEN DING 14, 60, 60, 4
IF schlussel(2) = 0 THEN DING 14, 540, 60, 2
IF schlussel(3) = 0 THEN DING 14, 60, 360, 14
IF schlussel(4) = 0 THEN DING 14, 540, 360, 1
TOR 300, 150, 14
TOR 210, 210, 1
TOR 390, 210, 2
TOR 300, 240, 4
IF feld(24, 6) <> 0 THEN DING 7, 240, 60, 4
IF feld(30, 6) <> 0 THEN DING 7, 300, 60, 2
IF feld(36, 6) <> 0 THEN DING 7, 360, 60, 14
IF feld(42, 6) <> 0 THEN DING 7, 420, 60, 1
weiter7:
IF lives > 28 THEN lives = 28
LINE (2, 457)-(123, 472), 5, B
FOR balken = 3 TO lives * 4
LINE (balken, 458)-(balken + 10, 471), 9, BF
NEXT balken
FOR leer = balken + 10 TO 118
LINE (leer, 458)-(leer + 4, 471), 0, BF
NEXT leer
IF feld(30, 30) <> 0 THEN DING 10, 300, 300, 1
END IF
IF x = 300 AND y = 60 THEN
CLS
SELECT CASE r
CASE 1
PUT (x, y), nepomuk1
CASE 2
PUT (x, y), nepomuk2
CASE 3
PUT (x, y), nepomuk3
CASE 4
PUT (x, y), nepomuk4
END SELECT
IF feld(30, 30) <> 0 THEN DING 10, 300, 300, 1
IF lives > 28 THEN lives = 28
LINE (2, 457)-(123, 472), 5, B
FOR balken = 3 TO lives * 4
LINE (balken, 458)-(balken + 10, 471), 9, BF
NEXT balken
FOR leer = balken + 10 TO 118
LINE (leer, 458)-(leer + 4, 471), 0, BF
NEXT leer
END IF
IF x = 360 AND y = 60 THEN BLACK 300, 120, 4
IF x = 420 AND y = 60 THEN
BLACK 120, 60, 2
BLACK 480, 60, 2
BLACK 120, 360, 2
BLACK 480, 360, 2
END IF
END SELECT
END SUB

SUB TISCH (x, y, z)

feld(x / 10, y / 10) = 26
IF z = 1 THEN GOSUB Tisch1
IF z = 2 THEN GOSUB Tisch2

Tisch1:
COLOR 6
LINE (x + 0, y + 0)-(x + 60, y + 30), , BF
COLOR 0
LINE (x + 6, y + 0)-(x + 54, y + 30), , B
LINE (x + 6, y + 6)-(x + 54, y + 6)
LINE (x + 6, y + 12)-(x + 54, y + 12)
LINE (x + 6, y + 24)-(x + 54, y + 24)
LINE (x + 6, y + 18)-(x + 54, y + 18)
GOTO 3

Tisch2:
LINE (x + 0, y + 0)-(x + 30, y + 60), 6, BF
LINE (x + 0, y + 6)-(x + 30, y + 54), 0, B
LINE (x + 6, y + 6)-(x + 6, y + 54), 0
LINE (x + 12, y + 6)-(x + 12, y + 54), 0
LINE (x + 18, y + 6)-(x + 18, y + 54), 0
LINE (x + 24, y + 6)-(x + 24, y + 54), 0

3 END SUB

SUB TOR (x, y, f)
feld(x / 10, y / 10) = 21
COLOR f
LINE (x + 0, y + 0)-(x + 30, y + 30), , B
PAINT (x + 12, y + 12)
CIRCLE (x + 15, y + 12), 4, 0
PAINT (x + 15, y + 12), 0
LINE (x + 13, y + 20)-(x + 17, y + 15), 0, B
PAINT (x + 14, y + 17), 0
END SUB

SUB TRESOR (x, y, z)

feld(x / 10, y / 10) = 23
IF z = 1 THEN GOSUB Tresor1
IF z = 10 THEN GOSUB Tresor10
IF z = 2 THEN GOSUB Tresor2
IF z = 20 THEN GOSUB Tresor20
IF z = 3 THEN GOSUB Tresor3
IF z = 30 THEN GOSUB Tresor30
IF z = 4 THEN GOSUB Tresor4
IF z = 40 THEN GOSUB Tresor40

Tresor1:
COLOR 8
LINE (x + 2, y + 2)-(x + 28, y + 20), , B
PAINT (x + 15, y + 15)
LINE (x + 4, y + 4)-(x + 26, y + 18), 7, B
PSET (x + 15, y + 21)
LINE (x + 11, y + 22)-(x + 19, y + 22)
GOTO 7

Tresor10:
COLOR 8
LINE (x + 2, y + 2)-(x + 28, y + 20), , B
PAINT (x + 15, y + 15)
LINE (x + 4, y + 4)-(x + 26, y + 18), 7, B
LINE (x + 10, y + 21)-(x + 10, y + 30)
LINE (x + 11, y + 22)-(x + 11, y + 30), , B
PSET (x + 9, y + 26)
LINE (x + 8, y + 23)-(x + 8, y + 29)
GOTO 7

Tresor2:
COLOR 8
LINE (x + 2, y + 28)-(x + 28, y + 10), , B
PAINT (x + 15, y + 15)
LINE (x + 4, y + 26)-(x + 26, y + 12), 7, B
PSET (x + 15, y + 9)
LINE (x + 11, y + 8)-(x + 19, y + 8)
GOTO 7

Tresor20:
COLOR 8
LINE (x + 2, y + 28)-(x + 28, y + 10), , BF
LINE (x + 4, y + 26)-(x + 26, y + 12), 7, B
LINE (x + 10, y + 9)-(x + 10, y + 0)
LINE (x + 11, y + 8)-(x + 11, y + 0), , B
PSET (x + 9, y + 4)
LINE (x + 8, y + 7)-(x + 8, y + 1)
GOTO 7

Tresor3:
COLOR 8
LINE (x + 2, y + 2)-(x + 20, y + 28), , BF
LINE (x + 4, y + 4)-(x + 18, y + 26), 7, B
PSET (x + 21, y + 15)
LINE (x + 22, y + 11)-(x + 22, y + 19)
GOTO 7

Tresor30:
COLOR 8
LINE (x + 2, y + 2)-(x + 20, y + 28), , BF
LINE (x + 4, y + 4)-(x + 18, y + 26), 7, B
LINE (x + 21, y + 10)-(x + 30, y + 10)
LINE (x + 22, y + 11)-(x + 29, y + 12), , B
PSET (x + 26, y + 9)
LINE (x + 23, y + 8)-(x + 29, y + 8)
GOTO 7

Tresor4:
COLOR 8
LINE (x + 28, y + 2)-(x + 10, y + 28), , BF
LINE (x + 26, y + 4)-(x + 12, y + 26), 7, B
PSET (x + 9, y + 15)
LINE (x + 8, y + 11)-(x + 8, y + 19)
GOTO 7

Tresor40:
COLOR 8
LINE (x + 28, y + 2)-(x + 10, y + 28), , BF
LINE (x + 26, y + 4)-(x + 12, y + 26), 7, B
LINE (x + 9, y + 10)-(x + 0, y + 10)
LINE (x + 8, y + 11)-(x + 1, y + 12), , B
PSET (x + 4, y + 9)
LINE (x + 7, y + 8)-(x + 1, y + 8)

7 END SUB

SUB WASSER (x, y)
feld(x / 10, y / 10) = 27
COLOR 1
LINE (x + 0, y + 0)-(x + 30, y + 30), 1, BF
END SUB

