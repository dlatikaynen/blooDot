DECLARE SUB INVENTAR ()
DECLARE SUB saure (x!, y!, z!)
DECLARE SUB bombe (x!, y!, z!)
DECLARE SUB keil (x!, y!, r!)
DECLARE SUB monster (x!, y!, z!, a!)
DECLARE SUB delay (dauer%)
DECLARE SUB EXPLOSION (xp!, yp!, rad!, col!)
DECLARE SUB kern (x!, y!)
DECLARE SUB NEPOMUK ()
DECLARE SUB TIMERS (level%, ZUSATZ!)
DECLARE SUB schleuse (x!, y!, z!)
DECLARE SUB tresor (x!, y!, z!)
DECLARE SUB eis (x!, y!)
DECLARE SUB wasser (a!, B!)
DECLARE SUB tor (x!, y!, f!)
DECLARE SUB mauer (x!, y!)
DECLARE SUB fels (x!, y!)
DECLARE SUB ding (art!, x!, y!, f!)
DECLARE SUB LEVELS (level%)
DECLARE SUB black (a!, B!, z!)
DECLARE SUB tisch (x!, y!, z!)
CLS
SCREEN 12
COLOR 15
mauergrosse! = 4 + INT(((PMAP(30, 0) - PMAP(0, 0) + 1) * (1) + 7) / 8) * 4 * (PMAP(30, 1) - PMAP(0, 1) + 1)
skerngrosse! = 4 + INT(((PMAP(29, 0) - PMAP(1, 0) + 1) * (1) + 7) / 8) * 4 * (PMAP(29, 1) - PMAP(1, 1) + 1)

DIM SHARED level AS INTEGER
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
DIM SHARED vieht(30)
DIM SHARED k, l, m, wind

NEPOMUK

k = 1
x = 0
y = 0
r = 0
s1 = 0
s2 = 0

OPEN "ubergang.dat" FOR INPUT AS #1
INPUT #1, level, lives, score, scorem, schusse
CLOSE #1

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

erstes:

CLS
SCREEN 12
FOR lx = 0 TO 600 STEP 30
mauer lx, 0: feld(lx / 10, 0) = 1
mauer lx, 420: feld(lx / 10, 42) = 1
NEXT lx
FOR ly = 30 TO 390 STEP 30
mauer 0, ly: feld(0, ly / 10) = 1
mauer 600, ly: feld(60, ly / 10) = 1
NEXT ly
x = 30: y = 30: r = 2
LEVELS level
GOSUB kopfzeile
GOSUB kp2
ON TIMER(1) GOSUB maintimer
TIMER ON
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
          CASE "S": GOTO LEVELENDE
          CASE "s": GOTO LEVELENDE
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
               IF a$ = "" THEN PRINT "Ist halt dumm, wenn man zu faul ist, um eine Taste zu dÅcken !" ELSE PRINT "Danke !"
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
IF level = 29 OR level = 37 THEN GOSUB spezialabfrage
IF (feld(x / 10, y / 10)) = 0 THEN GOTO leeresfeld
IF (feld(x / 10, y / 10)) = 1 THEN GOSUB GEGENMAUER: GOTO leeresfeld
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

GEGENMAUER:

IF r = 1 THEN x = x + 30
IF r = 2 THEN x = x - 30
IF r = 3 THEN y = y - 30
IF r = 4 THEN y = y + 30

RETURN

sonstiges:

ON (feld(x / 10, y / 10)) GOSUB g1, g2, g3, g4, g5, g6, g7, g8, g9, g10, g11, g12, g13, g14, g15, g16, g17, g18, g19, g20, g21, g22, g23, g24, g25, g26, g27, g28, g29, g30, g31, g32, g33, g34, g35, g36, g37, g38, g39, g40, g41, g42, g43, g44, g45
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
IF level = 29 THEN black 540, 240, 3: black 540, 270, 3: black 540, 300, 3: black 540, 330, 3: black 540, 360, 3: black 540, 390, 3: ding 1, 540, 240, 0
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
mauer a, 0
mauer a, 420
NEXT
FOR a = 0 TO 420 STEP 30
mauer 0, a
mauer 600, a
NEXT
FOR a = 30 TO 570 STEP 30
FOR B = 30 TO 390 STEP 30
trankl14a = feld(a / 10, B / 10)
SELECT CASE trankl14a
CASE 1
mauer a, B
CASE 2 TO 18
ding trankl14a - 1, a, B, 1
CASE 19
eis a, B
CASE 21
tor a, B, 1
CASE 25
fels a, B
CASE ELSE
END SELECT
NEXT
NEXT
IF schlussel(1) = 0 THEN ding 14, 60, 60, 4
IF schlussel(2) = 0 THEN ding 14, 540, 60, 2
IF schlussel(3) = 0 THEN ding 14, 60, 360, 14
IF schlussel(4) = 0 THEN ding 14, 540, 360, 1
tor 300, 150, 14
tor 210, 210, 1
tor 390, 210, 2
tor 300, 240, 4
IF feld(24, 6) <> 0 THEN ding 7, 240, 60, 4
IF feld(30, 6) <> 0 THEN ding 7, 300, 60, 2
IF feld(36, 6) <> 0 THEN ding 7, 360, 60, 14
IF feld(42, 6) <> 0 THEN ding 7, 420, 60, 1
FOR i = 1 TO 4
IF feld(viehx(i) / 10, viehy(i) / 10) <> 0 THEN monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
END SELECT
feld(x / 10, y / 10) = 0
art = 10
RETURN
g12:
feld(x / 10, y / 10) = 0
TIMER OFF
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
IF level = 26 OR level = 29 OR level = 31 OR level = 36 THEN GOSUB standardschlussel ELSE GOSUB spezialschlussel
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
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(x / 10, y / 10) = 0
GOSUB wassertod
RETURN
g21:
IF level = 26 OR level = 29 OR level = 31 OR level = 36 THEN GOSUB standardtor ELSE GOSUB spezialtor
art = 0
RETURN
g22:
art = 0
GOSUB SCHA
RETURN
g23:
art = 0
IF level = 13 AND zettel(1) = 1 THEN GOSUB tresor ELSE GOSUB GEGENMAUER
RETURN
g24:
art = 0
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(x / 10, y / 10) = 0
GOSUB wassertod
RETURN
g25:
art = 0
GOSUB GEGENMAUER
RETURN
g26:
art = 0
GOSUB GEGENMAUER
RETURN
g27:
art = 0
GOSUB wassertod
RETURN
g28:
TIMER OFF
RESTORE g28d
art = 0
FOR j = 200 TO 0 STEP -50
READ col
PLAY "MBO0L32EFGEFDC"
FOR i = 0 TO j STEP .25
CIRCLE (x + 15, y + 15), i, col
NEXT i
FOR i = j TO 0 STEP (-.15)
CIRCLE (x + 15, y + 15), i, 0
NEXT i
NEXT j
g28d: DATA 12, 9, 14, 11, 10
GOSUB kp2
GOSUB kopfzeile
GOSUB wassertod
RETURN
g29:
TIMER OFF
art = 0
PLAY "MBO0L32EFGEFDC"
FOR i = 0 TO 400 STEP .25
CIRCLE (x + 15, y + 15), i, 15
NEXT i
FOR i = 400 TO 0 STEP (-.15)
CIRCLE (x + 15, y + 15), i, 0
NEXT i
black x, y, 3
IF level = 29 OR level = 30 OR level = 33 THEN TIMER ON
GOSUB kp2
GOSUB kopfzeile
RETURN
g30:
art = 0
TIMER OFF
FOR i = 1 TO 300
spoy = INT(RND * 29) + 1
spox = INT(RND * 29) + 1
sbig = INT(RND * 3)
CIRCLE (x + spox, y + spoy), sbig, 15
delay 50
NEXT i
SLEEP 1
GOSUB leeresfeld
GOSUB wassertod
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
g35:
art = 0
IF r = 3 THEN GOSUB wassertod ELSE GOSUB GEGENMAUER
RETURN
g36:
art = 0
IF r = 4 THEN GOSUB wassertod ELSE GOSUB GEGENMAUER
RETURN
g37:
art = 0
IF r = 2 THEN GOSUB wassertod ELSE GOSUB GEGENMAUER
RETURN
g38:
art = 0
IF r = 1 THEN GOSUB wassertod ELSE GOSUB GEGENMAUER
RETURN
g39:
art = 0
black x, y, 3
TIMER OFF
FOR i = 600 TO 900 STEP 10
SOUND i, 1
NEXT i
FOR i = 900 TO 400 STEP -10
SOUND i, 1
NEXT i
DIM blackpixel(64)
LINE (0, 0)-(1, 1), 0, BF
GET (0, 0)-(1, 1), blackpixel
FOR i = 1 TO 61700
PUT (INT(RND * 628), INT(RND * 448)), blackpixel, PSET
NEXT i
ERASE blackpixel
COLOR 15
TIMER ON
feld(x / 10, y / 10) = 0
black x, y, 4
RETURN
g40:
RETURN
g41:
art = 20
ON r GOSUB v1, v2, v3, v4
RETURN
v1:
IF feld((x - 30) / 10, y / 10) = 0 OR feld((x - 30) / 10, y / 10) = 20 THEN black x, y, 4: ding 20, x - 30, y, 0 ELSE GOSUB GEGENMAUER
RETURN
v2:
IF feld((x + 30) / 10, y / 10) = 0 OR feld((x + 30) / 10, y / 10) = 20 THEN black x, y, 4: ding 20, x + 30, y, 0 ELSE GOSUB GEGENMAUER
RETURN
v3:
IF feld(x / 10, (y + 30) / 10) = 0 OR feld(x / 10, (y + 30) / 10) = 20 THEN black x, y, 4: ding 20, x, y + 30, 0 ELSE GOSUB GEGENMAUER
RETURN
v4:
IF feld(x / 10, (y - 30) / 10) = 0 OR feld(x / 10, (y - 30) / 10) = 20 THEN black x, y, 4: ding 20, x, y - 30, 0 ELSE GOSUB GEGENMAUER
RETURN
g42:
art = 0
GOSUB genommen
GOSUB wassertod
RETURN
g43:
TIMER OFF
feld(x / 10, y / 10) = 0
black x, y, 4
art = 43
GOSUB genommen
IF level = 37 THEN wind = 2
TIMER ON
RETURN
g44:
RETURN
g45:
RETURN

wassertod:
BEEP
RETURN

maintimer:
IF wind <> 0 THEN
IF wind = 1 THEN
GOSUB leeresfeld
x = x - 30: r = 1
GOSUB abfrage
END IF
IF wind = 2 THEN
GOSUB leeresfeld
x = x + 30: r = 2
GOSUB abfrage
END IF
IF wind = 3 THEN
GOSUB leeresfeld
y = y + 30: r = 3
GOSUB abfrage
END IF
IF wind = 4 THEN
GOSUB leeresfeld
y = y - 30: r = 4
GOSUB abfrage
END IF
END IF

CALL TIMERS(level%, 0)
RETURN
 
genommen:
IF art = 43 THEN GOSUB win
ON art GOSUB Mun, sch, her, kre, piz, kuc, tra, rum, eis, glu, Uhr, reg, zet, slu, sei, kir, pem
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
PLAY "T255O4L4eeddccba               "
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
eis:
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
win:
PLAY "T255O4L64ddagagagagagagag"
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
IF schlussel(1) = 1 THEN GOTO LEVELENDE
GOSUB GEGENMAUER
RETURN

standardschlussel:
schlussel(1) = 1
RETURN

spezialtor:

SELECT CASE level

CASE 27:
IF x = 570 AND y = 30 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 570 AND y = 30 AND schlussel(1) = 1 THEN GOTO LEVELENDE
IF x = 120 AND y = 90 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 120 AND y = 90 AND schlussel(2) = 1 THEN GOSUB SCHA

CASE 28
IF x = 30 AND y = 210 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 30 AND y = 210 AND schlussel(1) = 1 THEN GOSUB SCHA
IF x = 30 AND y = 30 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 30 AND y = 30 AND schlussel(2) = 1 THEN GOTO LEVELENDE

CASE 30
IF x = 480 AND y = 390 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 480 AND y = 390 AND schlussel(1) = 1 THEN GOTO LEVELENDE
IF x = 570 AND y = 360 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 570 AND y = 360 AND schlussel(2) = 1 THEN GOSUB SCHA
IF x = 30 AND y = 270 AND schlussel(3) = 0 THEN GOSUB GEGENMAUER
IF x = 30 AND y = 270 AND schlussel(3) = 1 THEN GOSUB SCHA
IF x = 540 AND y = 30 AND schlussel(4) = 0 THEN GOSUB GEGENMAUER
IF x = 540 AND y = 30 AND schlussel(4) = 1 THEN GOSUB SCHA

CASE 32
IF x = 570 AND y = 60 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 570 AND y = 390 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 570 AND y = 60 AND schlussel(1) = 1 THEN GOSUB SCHA
IF x = 570 AND y = 390 AND schlussel(2) = 1 THEN GOTO LEVELENDE

CASE 33
IF x = 240 AND y = 300 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 240 AND y = 240 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 240 AND y = 180 AND schlussel(3) = 0 THEN GOSUB GEGENMAUER
IF x = 270 AND y = 150 AND schlussel(4) = 0 THEN GOSUB GEGENMAUER
IF x = 330 AND y = 150 AND schlussel(5) = 0 THEN GOSUB GEGENMAUER
IF x = 360 AND y = 180 AND schlussel(6) = 0 THEN GOSUB GEGENMAUER
IF x = 360 AND y = 240 AND schlussel(7) = 0 THEN GOSUB GEGENMAUER
IF x = 330 AND y = 270 AND schlussel(8) = 0 THEN GOSUB GEGENMAUER
IF x = 300 AND y = 240 AND schlussel(9) = 0 THEN GOSUB GEGENMAUER
IF x = 240 AND y = 300 AND schlussel(1) = 1 THEN GOSUB SCHA
IF x = 240 AND y = 240 AND schlussel(2) = 1 THEN GOSUB SCHA
IF x = 240 AND y = 180 AND schlussel(3) = 1 THEN GOSUB SCHA
IF x = 270 AND y = 150 AND schlussel(4) = 1 THEN GOSUB SCHA
IF x = 330 AND y = 150 AND schlussel(5) = 1 THEN GOSUB SCHA
IF x = 360 AND y = 180 AND schlussel(6) = 1 THEN GOSUB SCHA
IF x = 360 AND y = 240 AND schlussel(7) = 1 THEN GOSUB SCHA
IF x = 330 AND y = 270 AND schlussel(8) = 1 THEN GOSUB SCHA
IF x = 300 AND y = 240 AND schlussel(9) = 1 THEN GOTO LEVELENDE
                                         
CASE 34
IF x = 300 AND y = 330 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 90 AND y = 150 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 570 AND y = 360 AND schlussel(3) = 0 THEN GOSUB GEGENMAUER
IF x = 30 AND y = 390 AND schlussel(4) = 0 THEN GOSUB GEGENMAUER
IF x = 300 AND y = 330 AND schlussel(1) = 1 THEN GOSUB l34t1
IF x = 90 AND y = 150 AND schlussel(2) = 1 THEN GOSUB SCHA
IF x = 570 AND y = 360 AND schlussel(3) = 1 THEN GOSUB l34t3
IF x = 30 AND y = 390 THEN GOTO LEVELENDE

CASE 35
IF x = 30 AND y = 390 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 120 AND y = 390 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 210 AND y = 390 AND schlussel(3) = 0 THEN GOSUB GEGENMAUER
IF x = 300 AND y = 390 AND schlussel(4) = 0 THEN GOSUB GEGENMAUER
IF x = 390 AND y = 390 AND schlussel(5) = 0 THEN GOSUB GEGENMAUER
IF x = 480 AND y = 390 AND schlussel(6) = 0 THEN GOSUB GEGENMAUER
IF x = 540 AND y = 390 AND schlussel(7) = 0 THEN GOSUB GEGENMAUER
IF x = 570 AND y = 360 AND schlussel(8) = 0 THEN GOSUB GEGENMAUER
IF x = 30 AND y = 390 AND schlussel(1) = 1 THEN GOTO LEVELENDE
IF x = 120 AND y = 390 AND schlussel(2) = 1 THEN GOSUB SCHA
IF x = 210 AND y = 390 AND schlussel(3) = 1 THEN GOSUB SCHA
IF x = 300 AND y = 390 AND schlussel(4) = 1 THEN GOSUB SCHA
IF x = 390 AND y = 390 AND schlussel(5) = 1 THEN GOSUB SCHA
IF x = 480 AND y = 390 AND schlussel(6) = 1 THEN GOSUB SCHA
IF x = 540 AND y = 390 AND schlussel(7) = 1 THEN GOSUB SCHA
IF x = 570 AND y = 360 AND schlussel(8) = 1 THEN GOSUB SCHA

CASE 37
IF x = 30 AND y = 240 AND schlussel(1) = 0 THEN GOSUB GEGENMAUER
IF x = 240 AND y = 30 AND schlussel(2) = 0 THEN GOSUB GEGENMAUER
IF x = 330 AND y = 30 AND schlussel(3) = 0 THEN GOSUB GEGENMAUER
IF x = 540 AND y = 390 AND schlussel(4) = 0 THEN GOSUB GEGENMAUER
IF x = 30 AND y = 240 AND schlussel(1) = 1 THEN GOTO LEVELENDE
IF x = 240 AND y = 30 AND schlussel(2) = 1 THEN GOSUB SCHA
IF x = 330 AND y = 30 AND schlussel(3) = 1 THEN GOSUB SCHA
IF x = 540 AND y = 390 AND schlussel(4) = 1 THEN GOSUB SCHA




CASE ELSE
END SELECT
RETURN

l34t1:
IF r = 3 THEN
GOSUB SCHA
IF feld(30, 36) = 15 THEN art = 14: GOSUB genommen: feld(30, 36) = 0: schlussel(2) = 1
END IF
IF r = 4 THEN y = y - 30
RETURN
l34t3:
IF r = 4 THEN
GOSUB SCHA
IF feld(57, 33) = 15 THEN art = 14: GOSUB genommen: feld(57, 33) = 0: schlussel(4) = 1
END IF
RETURN

spezialschlussel:
TIMERS level%, 1
RETURN

LEVELENDE:

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
IF UCASE$(inven$) = "J" THEN GOTO inventliste
IF UCASE$(inven$) = "N" THEN GOTO weiter2 ELSE GOTO inventarnein
weiter2:
GOSUB beleg
level = level + 1
k = k + 1
IF k = 11 THEN k = 1
GOTO erstes

inventliste:
INVENTAR
GOTO weiter2

nehmen:
IF level = 3 AND x = 480 AND y = 330 AND r = 2 THEN black 510, 330, 4: tisch 510, 300, 2: schlussel(2) = 1: feld(51, 33) = 1
IF level = 3 AND x = 510 AND y = 360 AND r = 4 THEN black 510, 330, 4: tisch 510, 300, 2: schlussel(2) = 1: feld(51, 33) = 1

GOTO taste

SCHA:
IF r = 2 THEN x = x + 30
IF r = 1 THEN x = x - 30
IF r = 3 THEN y = y + 30
IF r = 4 THEN y = y - 30
RETURN
sch1:
IF r = 1 THEN x = x - 30
IF r <> 1 THEN GOSUB GEGENMAUER
RETURN
sch2:
IF r = 2 THEN x = x + 30
IF r <> 2 THEN GOSUB GEGENMAUER
RETURN
sch3:
IF r = 3 THEN y = y + 30
IF r <> 3 THEN GOSUB GEGENMAUER
RETURN
sch4:
IF r = 4 THEN y = y - 30
IF r <> 4 THEN GOSUB GEGENMAUER
RETURN

trank:
SELECT CASE level

CASE 27
ding 14, 570, 390, 12

CASE 28
ding 11, 570, 60, 0

CASE 29
ding 4, 360, 30, 0

CASE 30
IF x = 570 AND y = 120 THEN
OPEN "timer30.da2" FOR INPUT AS #1
INPUT #1, schusse
CLOSE #1
KILL "timer30.da2"
GOSUB kp2
END IF
IF x = 540 AND y = 120 THEN
black 300, 30, 3
black 180, 270, 3
black 120, 330, 3
black 480, 180, 3
black 510, 210, 3
black 540, 300, 3
black 90, 60, 3
black 120, 180, 3
black 60, 90, 3
fels 210, 180
fels 510, 30
fels 480, 60
fels 330, 90
saure 120, 240, 12
saure 180, 240, 12
saure 60, 300, 12
saure 60, 330, 12
black 270, 90, 4
black 90, 120, 4
black 420, 60, 4: fels 420, 60: mauer 420, 30
black 480, 120, 2
black 420, 60, 4
black 540, 180, 3
bombe 540, 180, 2
END IF

CASE 36
IF schusse < 31 THEN schusse = 35

CASE 37
IF x = 300 AND y = 30 THEN ding 14, 150, 30, 14
IF x = 120 AND y = 390 THEN black 540, 30, 2
IF x = 180 AND y = 390 THEN
black 570, 120, 3
ding 20, 390, 150, 0
ding 20, 390, 210, 0
ding 20, 420, 180, 0
END IF

CASE ELSE
END SELECT
  
RETURN

tresor:
SELECT CASE level

CASE ELSE
END SELECT
RETURN

spezialabfrage:
SELECT CASE level

CASE 29
IF x = 300 AND y = 30 THEN x = 30: y = 300: r = 3
IF x = 60 AND y = 270 THEN x = 480: y = 60: r = 2

CASE 37
IF x = 570 AND y = 30 THEN x = 30: y = 270: r = 3
IF feld(39, 12) <> 43 AND x = 330 AND y = 390 THEN wind = 0: ding 22, 240, 90, 0

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
delay 500
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
delay 500
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
delay 500
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
delay 500
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
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 3: EXPLOSION xpo, ypo, 14, 4

CASE 21 - 23
REM

CASE 24                                         'KERN
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: EXPLOSION xpo, ypo, 14, 2: PUT (xpo, ypo), skern

CASE 25                                         'FELS
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 14, 2

CASE 26 - 27
REM

CASE 28
TIMER OFF
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 200, 4
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 200, 4
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 200, 4
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 200, 4
GOSUB leeresfeld
GOSUB kopfzeile
IF level = 28 THEN
IF feld(57, 6) = 0 OR feld(57, 6) = 20 THEN TIMER ON
END IF

CASE 29
TIMER OFF
IF rpo = 1 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 300, 4
IF rpo = 2 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 300, 4
IF rpo = 3 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 300, 4
IF rpo = 4 THEN feld(xpo / 10, ypo / 10) = 0: black xpo, ypo, 4: EXPLOSION xpo, ypo, 300, 4
GOSUB leeresfeld
GOSUB kopfzeile
TIMER ON

CASE ELSE
END SELECT
GOSUB kp2
RETURN

SUB black (a, B, z)
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

SUB bombe (x, y, z)
ON z GOSUB bombe1, bombe2
EXIT SUB

bombe1:
feld(x / 10, y / 10) = 28
CIRCLE (x + 20, y + 7), 5, 15
LINE (x + 20, y + 8)-(x + 30, y + 20), 0, BF
LINE (x + 25, y + 7)-(x + 25, y + 5), 12
PSET (x + 24, y + 4), 12: PSET (x + 22, y + 4), 14
PSET (x + 26, y + 4), 14: PSET (x + 23, y + 6), 14
PSET (x + 27, y + 6), 14: PSET (x + 23, y + 8), 14
PSET (x + 27, y + 8), 14: PSET (x + 21, y + 6), 14
PSET (x + 29, y + 6), 14: PSET (x + 25, y + 10), 14
PSET (x + 25, y + 3), 14
CIRCLE (x + 15, y + 19), 10, 8
PAINT (x + 15, y + 15), 8
LINE (x + 12, y + 15)-(x + 18, y + 6), 8, BF
RETURN

bombe2:
feld(x / 10, y / 10) = 29
CIRCLE (x + 20, y + 7), 5, 15
LINE (x + 10, y + 8)-(x + 30, y + 20), 0, BF
LINE (x + 25, y + 7)-(x + 25, y + 5), 12
PSET (x + 24, y + 4), 12: PSET (x + 22, y + 4), 14
PSET (x + 26, y + 4), 14: PSET (x + 23, y + 6), 14
PSET (x + 27, y + 6), 14: PSET (x + 23, y + 8), 14
PSET (x + 27, y + 8), 14: PSET (x + 21, y + 6), 14
PSET (x + 29, y + 6), 14: PSET (x + 25, y + 10), 14
PSET (x + 25, y + 3), 14
LINE (x + 12, y + 8)-(x + 18, y + 29), 12, BF
RETURN

END SUB

SUB delay (dauer%)
FOR i = 0 TO dauer%
NEXT i
END SUB

SUB ding (art, x, y, f)

feld(x / 10, y / 10) = art + 1
IF art = 18 THEN feld(x / 10, y / 10) = 39
IF art = 19 THEN feld(x / 10, y / 10) = 40
IF art = 20 THEN feld(x / 10, y / 10) = 41
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

CASE 18
LINE (x + 0, y + 0)-(x + 30, y + 30), 14, B
CIRCLE (x + 15, y + 15), 14, 14
CIRCLE (x + 15, y + 15), 13, f
PAINT STEP(0, 0), f
CIRCLE (x + 15, y + 15), 4, 14
LINE (x + 13, y + 12)-(x + 9, y + 4), 14
LINE (x + 17, y + 12)-(x + 21, y + 4), 14
PAINT (x + 15, y + 10), 14
LINE (x + 12, y + 18)-(x + 8, y + 26), 14
LINE (x + 18, y + 18)-(x + 22, y + 26), 14
LINE (x + 2, y + 16)-(x + 11, y + 16), 14
PAINT (x + 10, y + 18), 14
LINE (x + 28, y + 16)-(x + 19, y + 16), 14
PAINT (x + 20, y + 18), 14

CASE 19
feld(x / 10, y / 10) = 1
LINE (x, y)-(x + 30, y + 30), 14, B
IF f <> 0 THEN LINE (x + 1, y + 1)-(x + 29, y + 29), 12, BF
COLOR 14
LINE (x + 13, y + 28)-(x + 9, y + 24)
LINE (x + 13, y + 28)-(x + 17, y + 24)
LINE (x + 9, y + 24)-(x + 17, y + 24)
PAINT (x + 13, y + 27)
LINE (x + 15, y + 15)-(x + 13, y + 23)
LINE (x + 17, y + 15)-(x + 14, y + 23)
LINE (x + 16, y + 15)-(x + 13, y + 23)
LINE (x + 10, y + 15)-(x + 17, y + 16), , BF
LINE (x + 10, y + 15)-(x + 12, y + 3)
LINE (x + 11, y + 15)-(x + 18, y + 3)
LINE (x + 12, y + 3)-(x + 18, y + 3)
PAINT (x + 14, y + 4)
PSET (x + 11, y + 14)

CASE 20
LINE (x, y)-(x + 30, y + 30), 4, BF
LINE (x + 3, y + 3)-(x + 27, y + 27), 11, B
LINE (x + 5, y + 5)-(x + 25, y + 25), 11, B
LINE (x + 7, y + 7)-(x + 23, y + 23), 11, B
LINE (x + 9, y + 9)-(x + 21, y + 21), 11, B
LINE (x + 11, y + 11)-(x + 19, y + 19), 4, BF
LINE (x + 13, y + 13)-(x + 17, y + 17), 11, B

CASE 21
feld(x / 10, y / 10) = 42
ON f GOTO std1, std2, std3, std4, std5, std6, stdende

std1:
COLOR 7
LINE (0 + x, y + 5)-(x + 30, y + 5)
LINE (0 + x, y + 15)-(x + 30, y + 15)
LINE (0 + x, y + 25)-(x + 30, y + 25)
FOR i = 2 TO 28 STEP 4
LINE (x + i, y + 3)-(x + i, y + 7)
LINE (x + i, y + 13)-(x + i, y + 17)
LINE (x + i, y + 23)-(x + i, y + 27)
NEXT i
EXIT SUB

std2:
COLOR 7
LINE (5 + x, y + 0)-(x + 5, y + 30)
LINE (15 + x, y + 0)-(x + 15, y + 30)
LINE (25 + x, y + 0)-(x + 25, y + 30)
FOR i = 2 TO 28 STEP 4
LINE (x + 3, y + i)-(x + 7, y + i)
LINE (x + 13, y + i)-(x + 17, y + i)
LINE (x + 23, y + i)-(x + 27, y + i)
NEXT i
EXIT SUB

std3:
COLOR 7
LINE (0 + x, 5 + y)-(25 + x, 5 + y)
LINE (25 + x, 5 + y)-(25 + x, 30 + y)
LINE (0 + x, 15 + y)-(15 + x, 15 + y)
LINE (15 + x, 15 + y)-(15 + x, 30 + y)
LINE (0 + x, 25 + y)-(5 + x, 25 + y)
LINE (5 + x, 25 + y)-(5 + x, 30 + y)
FOR i = 2 TO 22 STEP 4
LINE (x + i, y + 3)-(x + i, y + 7)
LINE (x + 23, y + 6 + i)-(x + 27, y + 6 + i)
NEXT i
FOR i = 2 TO 12 STEP 4
LINE (x + i, y + 13)-(x + i, y + 17)
LINE (x + 13, y + 16 + i)-(x + 17, y + 16 + i)
NEXT i
LINE (x + 2, y + 23)-(x + 2, y + 27)
LINE (x + 3, y + 28)-(x + 7, y + 28)
EXIT SUB

std4:
COLOR 7
LINE (5 + x, 0 + y)-(5 + x, 25 + y)
LINE (5 + x, 25 + y)-(30 + x, 25 + y)
LINE (15 + x, 0 + y)-(15 + x, 15 + y)
LINE (15 + x, 15 + y)-(30 + x, 15 + y)
LINE (25 + x, 0 + y)-(25 + x, 5 + y)
LINE (25 + x, 5 + y)-(30 + x, 5 + y)
FOR i = 2 TO 22 STEP 4
LINE (x + 3, y + i)-(x + 7, y + i)
LINE (x + 6 + i, y + 23)-(x + 6 + i, y + 27)
NEXT i
FOR i = 2 TO 12 STEP 4
LINE (x + 13, y + i)-(x + 17, y + i)
LINE (x + 16 + i, y + 13)-(x + 16 + i, y + 17)
NEXT i
LINE (x + 23, y + 2)-(x + 27, y + 2)
LINE (x + 28, y + 3)-(x + 28, y + 7)
EXIT SUB

std5:
COLOR 7
LINE (25 + x, 0 + y)-(25 + x, 25 + y)
LINE (25 + x, 25 + y)-(0 + x, 25 + y)
LINE (15 + x, 0 + y)-(15 + x, 15 + y)
LINE (15 + x, 15 + y)-(0 + x, 15 + y)
LINE (5 + x, 0 + y)-(5 + x, 5 + y)
LINE (5 + x, 5 + y)-(0 + x, 5 + y)
FOR i = 2 TO 22 STEP 4
LINE (x + 27, y + i)-(x + 23, y + i)
LINE (x + i, y + 23)-(x + i, y + 27)
NEXT i
FOR i = 2 TO 12 STEP 4
LINE (x + 17, y + i)-(x + 13, y + i)
LINE (x + i, y + 13)-(x + i, y + 17)
NEXT i
LINE (x + 7, y + 2)-(x + 3, y + 2)
LINE (x + 2, y + 3)-(x + 2, y + 7)
EXIT SUB

std6:
COLOR 7
LINE (30 + x, 5 + y)-(5 + x, 5 + y)
LINE (5 + x, 5 + y)-(5 + x, 30 + y)
LINE (30 + x, 15 + y)-(15 + x, 15 + y)
LINE (15 + x, 15 + y)-(15 + x, 30 + y)
LINE (30 + x, 25 + y)-(25 + x, 25 + y)
LINE (25 + x, 25 + y)-(25 + x, 30 + y)
FOR i = 2 TO 22 STEP 4
LINE (x + 6 + i, y + 3)-(x + 6 + i, y + 7)
LINE (x + 7, y + 6 + i)-(x + 3, y + 6 + i)
NEXT i
FOR i = 2 TO 12 STEP 4
LINE (x + 17 + i, y + 13)-(x + 17 + i, y + 17)
LINE (x + 17, y + 16 + i)-(x + 13, y + 16 + i)
NEXT i
LINE (x + 28, y + 23)-(x + 28, y + 27)
LINE (x + 27, y + 28)-(x + 23, y + 28)
EXIT SUB

stdende:
feld(x / 10, y / 10) = 1
LINE (x, y)-(x + 30, y + 30), 14, B
IF f <> 0 THEN LINE (x + 1, y + 1)-(x + 29, y + 29), 12, BF
COLOR 14
LINE (x + 13, y + 28)-(x + 9, y + 24)
LINE (x + 13, y + 28)-(x + 17, y + 24)
LINE (x + 9, y + 24)-(x + 17, y + 24)
PAINT (x + 13, y + 27)
LINE (x + 15, y + 15)-(x + 13, y + 23)
LINE (x + 17, y + 15)-(x + 14, y + 23)
LINE (x + 16, y + 15)-(x + 13, y + 23)
LINE (x + 10, y + 15)-(x + 17, y + 16), , BF
LINE (x + 10, y + 15)-(x + 12, y + 3)
LINE (x + 11, y + 15)-(x + 18, y + 3)
LINE (x + 12, y + 3)-(x + 18, y + 3)
PAINT (x + 14, y + 4)
PSET (x + 11, y + 14)
EXIT SUB

CASE 22
feld(x / 10, y / 10) = 43
LINE (x + 0, y + 0)-(x + 30, y + 30), 9, B
IF f <> 0 THEN LINE (x + 1, y + 1)-(x + 29, y + 29), 4, BF
FOR i = 1 TO 15 STEP 2
LINE (x + 15, y + 15)-(x + i, y + 1), 11
LINE (x + 15, y + 15)-(x + 29 - i, y + 29), 11
LINE (x + 15, y + 15)-(x + 1, y + 29 - i), 11
LINE (x + 15, y + 15)-(x + 29, y + i), 11
NEXT i

CASE ELSE
END SELECT

END SUB

SUB eis (x, y)

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
delay 250
NEXT i

END SUB

SUB fels (x, y)

feld(x / 10, y / 10) = 25
COLOR 8
LINE (x + 0, y + 0)-(x + 30, y + 30), 8, BF
LINE (x + 2, y + 2)-(x + 28, y + 28), 7, B
LINE (x + 3, y + 3)-(x + 27, y + 27), 7, B
CIRCLE (x + 15, y + 15), 3, 7
PAINT STEP(0, 0), 7

END SUB

SUB INVENTAR

LINE (6, 6)-(624, 444), 0, BF

SLEEP
END SUB

SUB keil (x, y, r)
ON r GOSUB k1, k2, k3, k4
EXIT SUB

k1:
feld(x / 10, y / 10) = 35
LINE (x + 15, y + 1)-(x + 8, y + 30), 8
LINE (x + 15, y + 1)-(x + 23, y + 30), 8
LINE (x + 8, y + 30)-(x + 23, y + 30), 8
PAINT (x + 15, y + 15), 8
LINE (x + 15, y + 8)-(x + 10, y + 30), 7
LINE (x + 15, y + 8)-(x + 20, y + 30), 7
LINE (x + 10, y + 30)-(x + 20, y + 30), 7
PAINT (x + 15, y + 15), 7
LINE (x + 15, y + 11)-(x + 12, y + 30), 8
LINE (x + 15, y + 11)-(x + 18, y + 30), 8
LINE (x + 12, y + 30)-(x + 18, y + 30), 8
PAINT (x + 15, y + 29), 8
RETURN

k2:
feld(x / 10, y / 10) = 36
LINE (x + 15, y + 29)-(x + 8, y + 0), 8
LINE (x + 15, y + 29)-(x + 23, y + 0), 8
LINE (x + 8, y + 0)-(x + 23, y + 0), 8
PAINT (x + 15, y + 15), 8
LINE (x + 15, y + 22)-(x + 10, y + 0), 7
LINE (x + 15, y + 22)-(x + 20, y + 0), 7
LINE (x + 10, y + 0)-(x + 20, y + 0), 7
PAINT (x + 15, y + 15), 7
LINE (x + 15, y + 19)-(x + 12, y + 0), 8
LINE (x + 15, y + 19)-(x + 18, y + 0), 8
LINE (x + 12, y + 0)-(x + 18, y + 0), 8
PAINT (x + 15, y + 1), 8
RETURN

k3:
feld(x / 10, y / 10) = 37
LINE (x + 1, y + 15)-(x + 30, y + 8), 8
LINE (x + 1, y + 15)-(x + 30, y + 23), 8
LINE (x + 30, y + 8)-(x + 30, y + 23), 8
PAINT (x + 15, y + 15), 8
LINE (x + 8, y + 15)-(x + 30, y + 10), 7
LINE (x + 8, y + 15)-(x + 30, y + 20), 7
LINE (x + 30, y + 10)-(x + 30, y + 20), 7
PAINT (x + 15, y + 15), 7
LINE (x + 11, y + 15)-(x + 30, y + 12), 8
LINE (x + 11, y + 15)-(x + 30, y + 18), 8
LINE (x + 30, y + 12)-(x + 30, y + 18), 8
PAINT (x + 29, y + 15), 8
RETURN

k4:
feld(x / 10, y / 10) = 38
LINE (x + 29, y + 15)-(x + 0, y + 8), 8
LINE (x + 29, y + 15)-(x + 0, y + 23), 8
LINE (x + 0, y + 8)-(x + 0, y + 23), 8
PAINT (x + 15, y + 15), 8
LINE (x + 22, y + 15)-(x + 0, y + 10), 7
LINE (x + 22, y + 15)-(x + 0, y + 20), 7
LINE (x + 0, y + 10)-(x + 0, y + 20), 7
PAINT (x + 15, y + 15), 7
LINE (x + 19, y + 15)-(x + 0, y + 12), 8
LINE (x + 19, y + 15)-(x + 0, y + 18), 8
LINE (x + 0, y + 12)-(x + 0, y + 18), 8
PAINT (x + 1, y + 15), 8
RETURN

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

0 SUB LEVELS (level%)
SELECT CASE level%

CASE 26
FOR i = 0 TO 30 STEP 30
FOR j = i TO 540 STEP 120
mauer 600 - j, 60
mauer 60 + j, 120
mauer 600 - j, 180
mauer 60 + j, 240
mauer 600 - j, 300
mauer 60 + j, 360
ding 1, 150 + i, 300, 0
ding 1, 270 + i, 300, 0
ding 1, 390 + i, 300, 0
ding 1, 510 + i, 300, 0
NEXT j
NEXT i
mauer 30, 60
mauer 30, 180
mauer 30, 300
ding 1, 60, 300, 0
ding 16, 30, 240, 0
tor 30, 30, 15
ding 14, 570, 390, 15
ding 1, 30, 120, 0
ding 6, 150, 180, 0
ding 5, 360, 90, 0
ding 8, 480, 210, 0
ding 2, 300, 180, 0
ding 3, 300, 90, 0
ding 12, 60, 90, 0
OPEN "timer26.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), viehr(i)
monster viehx(i), viehy(i), 2, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 30: y = 210: r = 3

CASE 27
FOR i = 0 TO 90 STEP 30
mauer 30 + i, 120
mauer 480 + i, 90
mauer 360 + i, 210
mauer 540, 150 + i
mauer 540, 270 + i
wasser 120, 210 + i
mauer 90 + i, 360
NEXT i
FOR i = 0 TO 30 STEP 30
mauer 30 + i, 360
mauer 90, 210 + i
mauer 90, 300 + i
mauer 120, 30 + i
mauer 300 + i, 60
mauer 450, 60 + i
mauer 360, 120 + i
mauer 270 + i, 270
mauer 480 + i, 270
NEXT i
FOR i = 0 TO 180 STEP 60
mauer 180, 60 + i
mauer 300 + i, 360
mauer 330 + i, 330
wasser 330 + i, 270
NEXT i
FOR i = 0 TO 60 STEP 30
mauer 60 + i, 180
mauer 240, 30 + i
mauer 300, 90 + i
mauer 420 + i, 150
mauer 240, 240 + i
mauer 240 + i, 360
NEXT i
mauer 60, 240
mauer 60, 300
mauer 180, 300
mauer 510, 30
mauer 360, 270
mauer 420, 270
mauer 210, 390
wasser 60, 210
wasser 60, 330
wasser 120, 330
wasser 300, 240
wasser 390, 60
mauer 510, 270
tor 570, 30, 12
ding 14, 90, 270, 10
tor 120, 90, 10
ding 1, 90, 60, 0
ding 1, 60, 90, 0
ding 5, 60, 60, 0
keil 240, 120, 2: keil 300, 180, 2
keil 210, 180, 4: keil 240, 210, 1
keil 210, 300, 1: keil 180, 330, 3
keil 570, 60, 3: keil 540, 120, 3
ding 7, 30, 30, 11
schleuse 30, 360, 3
schleuse 210, 390, 2
schleuse 240, 360, 3
schleuse 270, 270, 3
schleuse 510, 270, 4
schleuse 540, 150, 1
ding 1, 60, 390, 0
ding 1, 120, 390, 0
ding 1, 420, 120, 0
ding 1, 300, 30, 0
ding 1, 150, 60, 0
ding 1, 210, 60, 0
ding 1, 150, 270, 0
ding 1, 330, 240, 0
ding 1, 150, 150, 0
ding 1, 30, 210, 0
ding 1, 30, 270, 0
ding 3, 360, 90, 0
ding 1, 510, 150, 0
ding 1, 420, 180, 0
ding 1, 330, 180, 0
OPEN "timer27.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), viehr(i)
monster viehx(i), viehy(i), 1, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 540: y = 30: r = 3

CASE 28
FOR i = 0 TO 30 STEP 30
FOR j = i TO 510 STEP 90
wasser 90 + j, 30
NEXT j
mauer 30 + i, 90
mauer 120 + i, 90
wasser 210 + i, 90
wasser 300 + i, 90
mauer 390 + i, 90
mauer 480 + i, 90
ding 1, 30 + i, 150, 0
ding 1, 120 + i, 150, 0
ding 1, 480 + i, 150, 0
ding 1, 90, 240 + i, 0
ding 2, 90, 330 + i, 0
ding 2, 90 + i, 390, 0
bombe 240, 270 + i, 1
bombe 240, 330 + i, 1
ding 1, 360 + i, 240, 0
ding 15, 480, 330 + i, 0
ding 16, 540, 270 + i, 0
mauer 570 + i, 90
NEXT i
ding 1, 330, 390, 0
ding 1, 360, 270, 0
ding 16, 150, 390, 0
ding 1, 210, 390, 0
FOR i = 0 TO 180 STEP 30
wasser 60 + i, 210
mauer 270, 210 + i
wasser 300 + i, 210
wasser 390 + i, 210
NEXT i
FOR i = 0 TO 120 STEP 30
bombe 30, 270 + i, 1
bombe 60, 270 + i, 1
bombe 120, 240 + i, 1
bombe 180, 270 + i, 1
bombe 300, 240 + i, 1
bombe 330, 240 + i, 1
bombe 390, 240 + i, 1
bombe 450, 240 + i, 1
bombe 510, 270 + i, 1
bombe 570, 240 + i, 1
ding 16, 150, 240 + i, 0
ding 1, 210, 240 + i, 0
NEXT i
ding 7, 150, 180, 9
ding 1, 60, 240, 0
mauer 90, 150
wasser 180, 150
mauer 270, 150
wasser 360, 150
mauer 450, 150
wasser 540, 150
bombe 570, 390, 1
ding 3, 540, 330, 0
ding 5, 510, 240, 0
ding 5, 480, 270, 0
ding 1, 480, 300, 0
ding 12, 480, 240, 0
ding 3, 180, 240, 0
ding 14, 540, 390, 6
tor 30, 210, 6
ding 14, 240, 240, 14
tor 30, 30, 14
schleuse 270, 390, 1
OPEN "timer28.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), vieht(i), viehr(i)
monster viehx(i), viehy(i), vieht(i), viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 420: y = 300: r = 4

CASE 29
FOR i = 0 TO 90 STEP 30
wasser 510, 30 + i
wasser 510, 150 + i
wasser 510, 270 + i
mauer 210 + i, 180
mauer 330, 180 + i
mauer 210, 210 + i
mauer 240 + i, 300
mauer 150 + i, 120
mauer 270 + i, 120
mauer 390, 120 + i
mauer 390, 240 + i
mauer 180 + i, 360
mauer 300 + i, 360
mauer 150, 150 + i
mauer 150, 270 + i
mauer 90, 300 + i
mauer 90, 180 + i
mauer 90, 60 + i
mauer 120 + i, 60
mauer 240 + i, 60
mauer 360 + i, 60
mauer 450, 90 + i
mauer 450, 210 + i
mauer 450, 300 + i
saure 30, 150 + i, 3
saure 60, 150 + i, 3
saure 30, 30 + i, 3
saure 60, 30 + i, 3
saure 90 + i, 30, 3
saure 150 + i, 30, 3
NEXT i
FOR i = 0 TO 30 STEP 30
mauer 30 + i, 270
mauer 270 + i, 30
ding 1, 30 + i, 360, 0
ding 1, 270, 210 + i, 0
NEXT i
schleuse 150, 330, 2
schleuse 270, 120, 4
schleuse 330, 210, 1
schleuse 330, 270, 2
schleuse 450, 390, 2
mauer 510, 390
schleuse 510, 390, 2
bombe 30, 330, 1
bombe 60, 330, 2
bombe 300, 270, 2
ding 3, 30, 390, 0
ding 7, 60, 390, 14
ding 16, 240, 210, 0
ding 16, 570, 60, 0
ding 16, 570, 150, 0
ding 16, 570, 240, 0
ding 16, 570, 330, 0
ding 1, 570, 90, 0
ding 1, 570, 180, 0
ding 1, 570, 270, 0
ding 1, 570, 360, 0
ding 5, 570, 120, 0
ding 5, 570, 300, 0
ding 9, 570, 210, 0
ding 9, 570, 390, 0
ding 14, 240, 240, 11
tor 570, 30, 11
schleuse 300, 30, 1
feld(30, 30) = 100
schleuse 30, 270, 3
schleuse 60, 270, 4
feld(6, 27) = 100
schleuse 450, 60, 2
OPEN "timer29.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), vieht(i), viehr(i)
monster viehx(i), viehy(i), vieht(i), viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 120: y = 300: r = 3

CASE 30
FOR i = 0 TO 60 STEP 30
mauer 60 + i, 150
mauer 150, 30 + i
mauer 270 + i, 270
mauer 180 + i, 300
mauer 240 + i, 330
mauer 30 + i, 210
mauer 480 + i, 360
mauer 330 + i, 210
mauer 330 + i, 120
mauer 420 + i, 150
mauer 390, 300 + i
NEXT i
FOR i = 0 TO 30 STEP 30
mauer 90, 90 + i
mauer 150 + i, 180
mauer 210 + i, 120
mauer 240 + i, 180
mauer 450 + i, 90
mauer 540, 60 + i
mauer 510 + i, 150
mauer 510, 270 + i
mauer 450, 360 + i
mauer 150 + i, 360
mauer 60 + i, 270
mauer 390 + i, 270
NEXT i
FOR i = 0 TO 60 STEP 60
mauer 60 + i, 30
mauer 150 + i, 240
mauer 360, 330 + i
mauer 480 + i, 240
NEXT i
mauer 360, 90
mauer 30, 90
mauer 90, 240
mauer 210, 60
mauer 570, 90
mauer 480, 120
mauer 300, 150
mauer 360, 180
mauer 180, 150
mauer 120, 300
mauer 90, 330
mauer 60, 360
mauer 300, 360
mauer 450, 300
mauer 570, 300
mauer 270, 210
mauer 510, 180
FOR i = 0 TO 120 STEP 120
mauer 360 + i, 30
mauer 240 + i, 30
mauer 150 + i, 90
mauer 300 + i, 60
mauer 120 + i, 390
mauer 450 + i, 210
NEXT i
OPEN "timer30.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), viehr(i)
monster viehx(i), viehy(i), 1, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
ding 18, 300, 240, 0
ding 18, 240, 240, 0
ding 14, 270, 240, 1
tor 480, 390, 1
ding 14, 60, 240, 7
tor 570, 360, 7
ding 14, 570, 60, 9
tor 30, 270, 9
ding 14, 30, 30, 15
tor 540, 30, 15
ding 1, 30, 60, 0
ding 1, 90, 60, 0
ding 1, 60, 90, 0
ding 1, 300, 30, 0
ding 1, 390, 60, 0
ding 1, 390, 90, 0
ding 1, 570, 150, 0
ding 1, 480, 180, 0
ding 1, 510, 210, 0
ding 1, 420, 210, 0
ding 1, 540, 300, 0
ding 1, 540, 390, 0
ding 1, 450, 330, 0
ding 1, 300, 300, 0
ding 1, 180, 270, 0
ding 1, 180, 390, 0
ding 1, 120, 330, 0
ding 1, 120, 180, 0
ding 3, 300, 180, 0
ding 3, 360, 360, 0
ding 7, 540, 120, 5
ding 7, 570, 120, 4
bombe 30, 150, 1
bombe 540, 180, 1
OPEN "timer30.da2" FOR OUTPUT AS #1
PRINT #1, schusse
CLOSE #1
schusse = 0
LINE (125, 451)-(629, 479), 0, BF
x = 360: y = 150: r = 3

CASE 31
FOR j = 30 TO 360 STEP 60
FOR i = 60 TO 540 STEP 60
ding 20, i + 30, 0 + j, 0
ding 20, i, 30 + j, 0
ding 20, i + 30, 390, 0
ding 20, 30, j, 0
ding 20, 30, 390, 0
NEXT i
NEXT j
tor 570, 360, 9
ding 14, 90, 60, 9
x = 540: y = 390: r = 4

CASE 32
FOR i = 60 TO 390 STEP 30
mauer 540, i
mauer 300, i
NEXT i
mauer 300, 30
black 300, 90, 1
black 300, 180, 1
black 300, 270, 1
black 300, 360, 1
black 300, 30, 2
ding 20, 270, 90, 0
ding 20, 270, 180, 0
ding 20, 270, 270, 0
ding 20, 270, 360, 0
ding 18, 360, 90, 0
ding 18, 360, 270, 0
ding 18, 360, 360, 0
bombe 360, 180, 2
ding 3, 480, 210, 0
tor 570, 60, 5
tor 570, 390, 10
ding 14, 30, 180, 5
ding 14, 30, 240, 10
saure 90, 120, 2
saure 150, 120, 2
saure 210, 120, 2
saure 150, 180, 2
saure 150, 210, 2
saure 150, 240, 2
saure 120, 210, 2
saure 180, 210, 2
saure 90, 300, 2
saure 90, 300, 2
saure 150, 300, 2
saure 210, 300, 2
ding 15, 90, 60, 0
ding 15, 90, 360, 0
FOR i = 30 TO 120 STEP 30
ding 16, 30, i, 0
ding 16, 30, i + 270, 0
NEXT i
OPEN "timer32.da1" FOR INPUT AS #1
FOR i = 1 TO 11
INPUT #1, viehx(i), viehy(i), viehr(i)
monster viehx(i), viehy(i), 1, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
FOR i = 12 TO 15
INPUT #1, viehx(i), viehy(i), viehr(i)
keil viehx(i), viehy(i), viehr(1)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 30: y = 210: r = 2

CASE 33
FOR i = 30 TO 390 STEP 30
mauer 90, i
mauer 150, i
mauer 390, i
NEXT i
schleuse 90, 360, 2
schleuse 150, 360, 2
mauer 30, 60: schleuse 30, 60, 3
mauer 60, 60: schleuse 60, 60, 3
mauer 120, 60: schleuse 120, 60, 3
black 90, 30, 2
black 150, 30, 2
fels 390, 60
fels 390, 360
mauer 390, 30
FOR i = 120 TO 300 STEP 30
mauer 210, i
mauer 480, i
NEXT i
mauer 390, 390
mauer 390, 30
mauer 480, 90
mauer 510, 90
mauer 480, 330
mauer 510, 330
FOR i = 210 TO 360 STEP 30
mauer i, 120
mauer i, 300
NEXT i
mauer 270, 180
mauer 300, 180
mauer 330, 180
mauer 270, 210
mauer 300, 210
mauer 330, 210
mauer 270, 240
mauer 330, 240
mauer 270, 270
tor 240, 300, 1
tor 240, 240, 2
tor 240, 180, 3
tor 270, 150, 4
tor 330, 150, 5
tor 360, 180, 6
tor 360, 240, 9
tor 330, 270, 10
tor 300, 240, 11
ding 14, 30, 390, 1
ding 14, 60, 390, 2
ding 14, 120, 390, 3
ding 14, 510, 30, 4
ding 14, 570, 30, 5
ding 14, 570, 210, 6
ding 14, 510, 390, 9
ding 14, 570, 390, 10
ding 14, 360, 360, 11
ding 1, 30, 300, 0
ding 1, 30, 330, 0
ding 1, 180, 300, 0
ding 1, 180, 330, 0
ding 1, 210, 330, 0
ding 1, 240, 330, 0
ding 1, 240, 360, 0
ding 1, 240, 390, 0
ding 1, 450, 390, 0
ding 1, 540, 90, 0
ding 1, 570, 90, 0
ding 1, 570, 120, 0
ding 1, 570, 150, 0
ding 1, 570, 300, 0
ding 16, 60, 300, 0
ding 16, 120, 330, 0
ding 16, 180, 390, 0
ding 16, 270, 360, 0
ding 16, 300, 360, 0
ding 16, 270, 60, 0
ding 16, 300, 30, 0
ding 16, 480, 390, 0
ding 16, 570, 240, 0
ding 16, 570, 330, 0
ding 5, 60, 330, 0
ding 5, 120, 300, 0
ding 5, 180, 270, 0
ding 5, 180, 360, 0
ding 5, 210, 390, 0
ding 5, 270, 30, 0
ding 5, 270, 390, 0
ding 5, 300, 390, 0
ding 5, 300, 60, 0
ding 5, 570, 270, 0
bombe 180, 60, 1
bombe 180, 240, 1
bombe 360, 390, 1
bombe 510, 60, 1
bombe 540, 180, 1
bombe 210, 60, 2
bombe 480, 360, 2
bombe 450, 30, 2
OPEN "timer33.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), viehr(i)
monster viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 360: y = 60: r = 2

CASE 34
FOR i = 0 TO 90 STEP 30
mauer i + 30, 180
mauer 60, i + 210
mauer 60, i + 300
mauer 180 + i, 150
mauer 120, 270 + i
mauer 180 + i, 300
mauer 270, 300 + i
mauer 270 + i, 390
mauer 390 + i, 330
mauer 480 + i, 270
mauer 330 + i, 60
mauer 420 + i, 120
mauer 330, 60 + i
NEXT i
FOR i = 0 TO 60 STEP 30
mauer 60, 60 + i
mauer 120 + i, 60
mauer 180, 120 + i
mauer 270, 90 + i
mauer 120 + i, 360
mauer 270, 180 + i
mauer 330, 210 + i
mauer 420, 180 + i
mauer 330, 330 + i
mauer 120 + i, 240
mauer 480 + i, 210
NEXT i
FOR i = 0 TO 30 STEP 30
mauer 240 + i, 240
mauer 330 + i, 210
mauer 180, 30 + i
mauer 420, 60 + i
mauer 540 + i, 30
mauer 60 + i, 120
mauer 390, 300 + i
mauer 540, 330 + i
NEXT i
mauer 570, 300
mauer 570, 150
mauer 570, 60
mauer 480, 360
mauer 360, 150
mauer 270, 30
mauer 480, 60
ding 20, 90, 60, 0
ding 20, 120, 210, 0
ding 20, 180, 330, 0
ding 20, 270, 180, 0
ding 20, 390, 150, 0
ding 20, 420, 30, 0
ding 20, 420, 270, 0
ding 20, 480, 390, 0
ding 20, 510, 240, 0
ding 20, 540, 120, 0
ding 14, 360, 30, 11
ding 14, 300, 360, 12
ding 14, 60, 30, 13
ding 14, 570, 330, 14
tor 300, 330, 11
tor 90, 150, 12
tor 570, 360, 13
tor 30, 390, 14
ding 18, 180, 90, 0
ding 3, 480, 150, 0
schleuse 60, 330, 1
ding 1, 30, 120, 0
OPEN "timer34.da1" FOR INPUT AS #1
FOR i = 1 TO 20
INPUT #1, viehx(i), viehy(i), viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
monster viehx(i), viehy(i), 2, viehr(i)
NEXT i
CLOSE #1
x = 90: y = 390: r = 2

CASE 35
FOR i = 0 TO 90 STEP 30
mauer 60 + i, 60
mauer 60 + i, 210
mauer 210 + i, 60
mauer 210 + i, 210
mauer 360 + i, 60
mauer 360 + i, 210
mauer 510 + i, 60
mauer 510 + i, 210
mauer 60 + i, 150
mauer 60 + i, 300
mauer 210 + i, 150
mauer 210 + i, 300
mauer 360 + i, 150
mauer 360 + i, 300
mauer 510 + i, 150
mauer 510 + i, 300
mauer 30 + i, 360
mauer 150 + i, 360
mauer 270 + i, 360
mauer 390 + i, 360
mauer 450 + i, 360
NEXT i
FOR i = 0 TO 30 STEP 30
mauer 60, 90 + i
mauer 60, 240 + i
mauer 150, 90 + i
mauer 150, 240 + i
mauer 210, 90 + i
mauer 210, 240 + i
mauer 300, 90 + i
mauer 300, 240 + i
mauer 360, 90 + i
mauer 360, 240 + i
mauer 450, 90 + i
mauer 450, 240 + i
mauer 510, 90 + i
mauer 510, 240 + i
NEXT i
FOR i = 90 TO 540 STEP 150
l = l + 1
ding 14, i + 30, 120, l
l = l + 1
ding 14, i + 30, 270, l
ding 20, i, 150, 0
ding 20, i, 300, 0
ding 9, i + 30, 30, 0
ding 1, i + 30, 180, 0
ding 9, i - 60, 90, 0
ding 9, i - 60, 270, 0
NEXT i
l = 0
FOR i = 0 TO 390 STEP 90
ding 16, i + 60, 390, 0
l = l + 1
tor i + 30, 390, l
NEXT i
tor 480, 390, 9
tor 540, 390, 10
tor 570, 360, 11
ding 9, 150, 30, 0
ding 9, 300, 330, 0
ding 9, 450, 330, 0
ding 14, 420, 270, 9
ding 14, 570, 270, 10
ding 14, 570, 120, 11
OPEN "timer35.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
monster viehx(i), viehy(i), 1, viehr(i)
NEXT i
CLOSE #1
x = 360: y = 330: r = 2

CASE 36
FOR i = 0 TO 90 STEP 30
mauer 30 + i, 60: mauer 120, 90 + i
mauer 60, 150 + i: mauer 180, 60 + i
mauer 120, 240 + i: mauer 180 + i, 180
mauer 240 + i, 240: mauer 360, 150 + i
mauer 390 + i, 180: mauer 150 + i, 300
mauer 270 + i, 300: mauer 480 + i, 390
mauer 480 + i, 270: mauer 450 + i, 120
mauer 420 + i, 60
NEXT i
FOR i = 0 TO 60 STEP 30
mauer 60 + i, 360: mauer 420, 330 + i
mauer 240, 60 + i: mauer 300, 60 + i
ding 1, 360, 330 + i, 0
NEXT i
FOR i = 0 TO 30 STEP 30
mauer 30 + i, 120: mauer 330, 30 + i
mauer 390, 30 + i: mauer 300 + i, 180
mauer 510 + i, 180: mauer 360 + i, 120
mauer 390 + i, 300: mauer 450 + i, 330
ding 1, 450, 210 + i, 0: ding 3, 30 + i, 30, 0
ding 1, 540, 210 + i, 0
NEXT i
mauer 270, 120: mauer 540, 60
mauer 570, 120: mauer 420, 240
mauer 210, 360: mauer 330, 360
FOR i = 0 TO 60 STEP 60
ding 1, 510, 30 + i, 0
ding 1, 570, 30 + i, 0
ding 1, 210, 330 + i, 0
ding 1, 330, 330 + i, 0
ding 1, 240, 210 + i, 0
NEXT i
ding 1, 120, 30, 0: ding 1, 150, 60, 0
ding 1, 210, 30, 0: ding 1, 450, 90, 0
ding 1, 420, 120, 0: ding 1, 480, 150, 0
ding 1, 150, 180, 0: ding 1, 120, 210, 0
ding 16, 270, 60, 0: ding 16, 570, 60, 0
ding 16, 90, 120, 0: ding 16, 330, 150, 0
ding 16, 390, 210, 0: ding 16, 480, 300, 0
ding 16, 570, 360, 0: ding 16, 60, 390, 0
ding 16, 450, 390, 0: ding 12, 270, 30, 0
ding 12, 540, 150, 0: ding 12, 90, 180, 0
ding 12, 510, 360, 0: ding 12, 150, 390, 0
ding 8, 30, 270, 0: ding 8, 30, 300, 0
ding 8, 30, 330, 0: ding 8, 540, 330, 0
ding 7, 390, 90, 9: ding 14, 390, 330, 12
tor 420, 30, 12: ding 3, 450, 360, 0
ding 2, 30, 90, 0: ding 9, 270, 150, 0
ding 2, 570, 210, 0: ding 9, 300, 270, 0
ding 2, 390, 270, 0: ding 9, 90, 330, 0
ding 2, 480, 360, 0
OPEN "timer36.da1" FOR INPUT AS #1
FOR i = 1 TO 30
INPUT #1, viehx(i), viehy(i), viehr(i)
monster viehx(i), viehy(i), 4, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 450: y = 30: r = 2

CASE 37
ding 21, 180, 60, 7
ding 21, 360, 210, 7
FOR i = 0 TO 120 STEP 30
ding 21, 210 + i, 60, 1
NEXT i
ding 21, 360, 60, 3
FOR i = 90 TO 180 STEP 30
ding 21, 360, i, 2
NEXT i
ding 22, 390, 120, 0
mauer 420, 120
ding 19, 450, 120, 1
ding 19, 540, 120, 1
ding 21, 480, 120, 1
ding 21, 510, 120, 1
FOR i = 0 TO 90 STEP 30
keil 450, 150 + i, 3
keil 450, 300 + i, 3
keil 540, 150 + i, 3
keil 540, 270 + i, 3
eis 330, 90 + i
mauer 360, 240 + i
mauer 210 + i, 330
mauer 60, 210 + i
mauer 90 + i, 210
mauer 60 + i, 150
NEXT i
FOR i = 0 TO 60 STEP 30
mauer 120, 30 + i
mauer 510 + i, 60
mauer 450, 30 + i
NEXT i
FOR i = 0 TO 30 STEP 30
mauer 60, 60 + i
mauer 180, 90 + i
mauer 30, 300 + i
mauer 360, 360 + i
bombe 270 + i, 210, 1
mauer 60 + i, 390 - i
mauer 120 + i, 330 - i
mauer 150 + i, 390 - i
mauer 210 + i, 180 - i
mauer 390, 30 + i
mauer 540 + i, 30
ding 1, 60 + i, 360 - i, 0
ding 1, 150 + i, 360 - i, 0
NEXT i
ding 1, 570, 120, 0
ding 1, 30, 60, 0
ding 1, 90, 30, 0
ding 1, 90, 120, 0
ding 1, 120, 180, 0
bombe 210, 90, 1
ding 3, 390, 390, 0
ding 3, 510, 150, 0
mauer 30, 120: mauer 30, 210
mauer 90, 270: mauer 210, 300
mauer 270, 300: mauer 300, 270
mauer 300, 360: mauer 210, 240
mauer 180, 30: mauer 270, 120
mauer 450, 270
schleuse 360, 390, 1
schleuse 450, 270, 2
schleuse 30, 120, 4
schleuse 180, 30, 2
schleuse 390, 30, 2
schleuse 570, 30, 2
schleuse 30, 300, 4
eis 300, 180
ding 18, 60, 120, 0
ding 18, 150, 60, 0
ding 20, 240, 240, 0
ding 20, 300, 390, 0
tor 30, 240, 9
tor 240, 30, 10
tor 330, 30, 11
tor 540, 390, 14
ding 14, 150, 30, 9
ding 14, 30, 390, 10
ding 14, 90, 390, 11
ding 7, 300, 30, 2
ding 7, 120, 390, 3
ding 7, 180, 390, 4
ding 3, 300, 300, 0
fels 570, 150
fels 570, 210
fels 570, 270
fels 570, 330
feld(3, 30) = 1: feld(57, 3) = 2
OPEN "timer37.da1" FOR INPUT AS #1
FOR i = 1 TO 21
INPUT #1, viehx(i), viehy(i), viehr(i)
monster viehx(i), viehy(i), 3, viehr(i)
feld(viehx(i) / 10, viehy(i) / 10) = 20
NEXT i
CLOSE #1
x = 30: y = 180: r = 3

CASE ELSE
END SELECT
END SUB

SUB mauer (x, y)

IF k = 1 THEN a = 2: B = 14
IF k = 2 THEN a = 9: B = 10
IF k = 3 THEN a = 14: B = 5
IF k = 4 THEN a = 6: B = 2
IF k = 5 THEN a = 13: B = 11
IF k = 6 THEN a = 4: B = 7
IF k = 7 THEN a = 9: B = 10
IF k = 8 THEN a = 12: B = 3
IF k = 9 THEN a = 14: B = 9
IF k = 10 THEN a = 10: B = 15

feld(x / 10, y / 10) = 1
LINE (x + 0, y + 0)-(x + 30, y + 30), 7, BF
LINE (x + 2, y + 2)-(x + 28, y + 28), 8, B
LINE (x + 3, y + 3)-(x + 27, y + 27), 8, B
LINE (x + 4, y + 4)-(x + 26, y + 26), a, BF
LINE (x + 8, y + 8)-(x + 22, y + 22), 8, B
LINE (x + 9, y + 9)-(x + 21, y + 21), 8, B
LINE (x + 10, y + 10)-(x + 20, y + 20), B, BF
LINE (x + 4, y + 4)-(x + 26, y + 26), 4
LINE (x + 4, y + 26)-(x + 26, y + 4), 4
LINE (x + 5, y + 4)-(x + 27, y + 26), 4
LINE (x + 3, y + 4)-(x + 25, y + 26), 4
LINE (x + 5, y + 26)-(x + 27, y + 4), 4
LINE (x + 3, y + 26)-(x + 25, y + 4), 4
END SUB

SUB monster (x, y, z, a)

IF z = 1 THEN GOTO supersau
IF z = 2 THEN GOTO vogel
IF z = 3 THEN GOTO killer
IF z = 4 THEN GOTO skelett

supersau:
IF a = 1 THEN GOTO sau1
IF a = 2 THEN GOTO sau2
IF a = 3 THEN GOTO sau3
IF a = 4 THEN GOTO sau4

sau1:
COLOR 13
CIRCLE (x + 15, y + 12), 11
PAINT (x + 15, y + 15)
LINE (x + 11, y + 27)-(x + 19, y + 15), , BF
PSET (x + 13, y + 27), 0: PSET (x + 17, y + 27), 0
CIRCLE (x + 12, y + 18), 2, 15, , , .15 * 3.141592654#
CIRCLE (x + 18, y + 18), 2, 15, , , .15 * 3.141592654#
LINE (x + 6, y + 10)-(x + 6, y + 14), 15, BF
LINE (x + 24, y + 10)-(x + 24, y + 14), 15, BF
PSET (x + 5, y + 9), 15: PSET (x + 25, y + 9), 15
PSET (x + 5, y + 15), 15: PSET (x + 25, y + 15), 15
PSET (x + 12, y + 18), 0: PSET (x + 18, y + 18), 0
LINE (x + 15, y + 10)-(x + 12, y + 5), 15
LINE (x + 15, y + 10)-(x + 15, y + 5), 15
LINE (x + 15, y + 10)-(x + 18, y + 5), 15
GOTO 10

sau2:
COLOR 13
CIRCLE (x + 15, y + 18), 11
PAINT (x + 15, y + 15)
LINE (x + 11, y + 3)-(x + 19, y + 15), , BF
PSET (x + 13, y + 3), 0: PSET (x + 17, y + 3), 0
CIRCLE (x + 12, y + 12), 2, 15, , , .15 * 3.141592654#
CIRCLE (x + 18, y + 12), 2, 15, , , .15 * 3.141592654#
LINE (x + 6, y + 20)-(x + 6, y + 14), 15, BF
LINE (x + 24, y + 20)-(x + 24, y + 14), 15, BF
PSET (x + 5, y + 21), 15: PSET (x + 25, y + 21), 15
PSET (x + 5, y + 15), 15: PSET (x + 25, y + 15), 15
PSET (x + 12, y + 12), 0: PSET (x + 18, y + 12), 0
LINE (x + 15, y + 20)-(x + 12, y + 25), 15
LINE (x + 15, y + 20)-(x + 15, y + 25), 15
LINE (x + 15, y + 20)-(x + 18, y + 25), 15
GOTO 10

sau3:
COLOR 13
CIRCLE (x + 12, y + 15), 11
PAINT (x + 15, y + 15)
LINE (x + 27, y + 11)-(x + 15, y + 19), , BF
PSET (x + 27, y + 13), 0: PSET (x + 27, y + 17), 0
CIRCLE (x + 18, y + 12), 2, 15, , , .9 * 3.141592654#
CIRCLE (x + 18, y + 18), 2, 15, , , .9 * 3.141592654#
LINE (x + 10, y + 6)-(x + 14, y + 6), 15, BF
LINE (x + 10, y + 24)-(x + 14, y + 24), 15, BF
PSET (x + 9, y + 5), 15: PSET (x + 9, y + 25), 15
PSET (x + 15, y + 5), 15: PSET (x + 15, y + 25), 15
PSET (x + 18, y + 12), 0: PSET (x + 18, y + 18), 0
LINE (x + 10, y + 15)-(x + 5, y + 12), 15
LINE (x + 10, y + 15)-(x + 5, y + 15), 15
LINE (x + 10, y + 15)-(x + 5, y + 18), 15
GOTO 10

sau4:
COLOR 13
CIRCLE (x + 18, y + 15), 11
PAINT (x + 15, y + 15)
LINE (x + 3, y + 11)-(x + 15, y + 19), , BF
PSET (x + 3, y + 13), 0: PSET (x + 3, y + 17), 0
CIRCLE (x + 12, y + 12), 2, 15, , , .9 * 3.141592654#
CIRCLE (x + 12, y + 18), 2, 15, , , .9 * 3.141592654#
LINE (x + 20, y + 6)-(x + 16, y + 6), 15, BF
LINE (x + 20, y + 24)-(x + 16, y + 24), 15, BF
PSET (x + 21, y + 5), 15: PSET (x + 21, y + 25), 15
PSET (x + 15, y + 5), 15: PSET (x + 15, y + 25), 15
PSET (x + 12, y + 12), 0: PSET (x + 12, y + 18), 0
LINE (x + 20, y + 15)-(x + 25, y + 12), 15
LINE (x + 20, y + 15)-(x + 25, y + 15), 15
LINE (x + 20, y + 15)-(x + 25, y + 18), 15
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
'.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.

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
black 0, 0, 0
CIRCLE (15, 15), 3, 0
PAINT STEP(0, 0), 3, 0
GET (1, 1)-(29, 29), skern
LINE (0, 0)-(30, 30), 0, BF

END SUB

SUB saure (x, y, z)
feld(x / 10, y / 10) = 30
LINE (x, y)-(x + 30, y + 30), z, BF
FOR i = 1 TO 15
spoy = INT(RND * 26) + 1
spox = INT(RND * 26) + 1
sbig = INT(RND * 3)
CIRCLE (x + spox, y + spoy), sbig, 15
NEXT i
END SUB

SUB schleuse (x, y, z)

IF level MOD 2 = 0 THEN a = 15 ELSE a = 9

IF z = 1 THEN GOSUB Schleuse1
IF z = 2 THEN GOSUB Schleuse2
IF z = 3 THEN GOSUB Schleuse3
IF z = 4 THEN GOSUB Schleuse4

Schleuse1:
feld(x / 10, y / 10) = 32
COLOR a
LINE (x + 7, y + 15)-(x + 14, y + 8)
LINE (x + 14, y + 8)-(x + 14, y + 23)
LINE (x + 7, y + 16)-(x + 14, y + 23)
LINE (x + 23, y + 18)-(x + 14, y + 12), , BF
PAINT (x + 8, y + 15)
EXIT SUB

Schleuse2:
feld(x / 10, y / 10) = 31
COLOR a
LINE (x + 23, y + 15)-(x + 16, y + 8)
LINE (x + 16, y + 8)-(x + 16, y + 23)
LINE (x + 23, y + 16)-(x + 16, y + 23)
LINE (x + 7, y + 18)-(x + 16, y + 12), , BF
PAINT (x + 22, y + 15)
EXIT SUB

Schleuse3:
feld(x / 10, y / 10) = 34
COLOR a
LINE (x + 15, y + 23)-(x + 8, y + 16)
LINE (x + 8, y + 16)-(x + 23, y + 16)
LINE (x + 16, y + 23)-(x + 23, y + 16)
LINE (x + 18, y + 7)-(x + 12, y + 16), , BF
PAINT (x + 15, y + 22)
EXIT SUB

Schleuse4:
feld(x / 10, y / 10) = 33
COLOR a
LINE (x + 15, y + 7)-(x + 8, y + 14)
LINE (x + 8, y + 14)-(x + 23, y + 14)
LINE (x + 16, y + 7)-(x + 23, y + 14)
LINE (x + 18, y + 23)-(x + 12, y + 14), , BF
PAINT (x + 15, y + 8)
END SUB

SUB TIMERS (level%, ZUSATZ)
IF ZUSATZ = 1 THEN GOTO sluss
IF ZUSATZ = 2 THEN GOTO trnk

SELECT CASE level%

CASE 26
 
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 27
IF wwasser(1) = 0 THEN wwasser(1) = 1 ELSE wwasser(1) = 0
IF wwasser(1) = 0 THEN
black 300, 300, 4
black 300, 330, 4
keil 300, 330, 1
keil 360, 300, 1
mauer 360, 330
black 420, 300, 4
black 420, 330, 4
keil 420, 330, 1
keil 480, 300, 1
mauer 480, 330
mauer 330, 360
keil 330, 390, 2
black 390, 360, 4
black 390, 390, 4
keil 390, 360, 2
mauer 450, 360
black 510, 360, 4
keil 450, 390, 2
black 510, 390, 4
keil 510, 360, 2
END IF
IF wwasser(1) = 1 THEN
keil 300, 300, 1
mauer 300, 330
black 360, 300, 4
black 360, 330, 4
keil 360, 330, 1
keil 420, 300, 1
mauer 420, 330
black 480, 300, 4
black 480, 330, 4
keil 480, 330, 1
black 330, 390, 4
black 330, 360, 4
keil 330, 360, 2
mauer 390, 360
keil 390, 390, 2
black 450, 360, 4
black 450, 390, 4
keil 450, 360, 2
mauer 510, 360
keil 510, 390, 2
END IF
FOR i = 35 TO 38
IF feld(x / 10, y / 10) = i THEN GOSUB wassertod2
NEXT i
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter3
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter3
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter3
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter3
weiter3:
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 28
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter4
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter4
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter4
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter4
weiter4:
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 29
FOR i = 0 TO 90 STEP 30
saure 30, 150 + i, 3
saure 60, 150 + i, 3
saure 30, 30 + i, 3
saure 60, 30 + i, 3
saure 90 + i, 30, 3
saure 150 + i, 30, 3
NEXT i
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter5
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter5
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter5
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), vieht(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter5
weiter5:
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 30
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter6
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter6
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter6
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter6
weiter6:
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i
IF l = 0 THEN l = 12 ELSE l = 0
IF feld(30, 24) = 39 THEN ding 18, 300, 240, l
IF feld(24, 24) = 39 THEN ding 18, 240, 240, l
IF feld(54, 12) = 0 OR feld(54, 12) = 20 THEN
saure 120, 240, 12
saure 180, 240, 12
saure 60, 300, 12
saure 60, 330, 12
END IF

CASE 32
saure 90, 120, 2
saure 150, 120, 2
saure 210, 120, 2
saure 150, 180, 2
saure 150, 210, 2
saure 150, 240, 2
saure 120, 210, 2
saure 180, 210, 2
saure 90, 300, 2
saure 90, 300, 2
saure 150, 300, 2
saure 210, 300, 2
FOR i = 1 TO 11
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

FOR i = 12 TO 15
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 4: black viehx(i) - 30, viehy(i), 4: keil viehx(i), viehy(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 2: black viehx(i), viehy(i) - 30, 4: keil viehx(i), viehy(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 3: black viehx(i) + 30, viehy(i), 4: keil viehx(i), viehy(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 1: black viehx(i), viehy(i) + 30, 4: keil viehx(i), viehy(i), viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

IF l = 12 THEN l = 0 ELSE l = 12
ding 18, 360, 90, l
ding 18, 360, 270, l
ding 18, 360, 360, l

CASE 33
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter8
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter8
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter8
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter8
weiter8:
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 34
IF l = 12 THEN l = 0 ELSE l = 12
IF feld(18, 9) = 39 THEN ding 18, 180, 90, l
FOR i = 1 TO 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter9
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter9
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter9
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 2, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20: GOTO weiter9
weiter9:
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 35
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 1, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 36
FOR i = 1 TO 30
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 4, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 4, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 4, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 4, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
EXPLOSION a, B, 15, 4
EXPLOSION a - 15, B - 15, 10, 2
feld(a / 10, B / 10) = 0
GOSUB wassertod2
END IF
NEXT i

CASE 37
IF l = 0 THEN l = 1 ELSE l = 0
IF feld(6, 12) = 39 THEN
IF l = 0 THEN ding 18, 60, 120, 12
IF l = 1 THEN ding 18, 60, 120, 0
END IF
IF feld(15, 6) = 39 THEN
IF l = 0 THEN ding 18, 150, 60, 0
IF l = 1 THEN ding 18, 150, 60, 12
END IF
FOR i = 1 TO 21
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x > viehx(i) AND feld((viehx(i) + 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) + 30: viehr(i) = 3: black viehx(i) - 30, viehy(i), 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y > viehy(i) AND feld(viehx(i) / 10, (viehy(i) + 30) / 10) = 0 THEN viehy(i) = viehy(i) + 30: viehr(i) = 1: black viehx(i), viehy(i) - 30, 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND x < viehx(i) AND feld((viehx(i) - 30) / 10, viehy(i) / 10) = 0 THEN viehx(i) = viehx(i) - 30: viehr(i) = 4: black viehx(i) + 30, viehy(i), 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF feld(viehx(i) / 10, viehy(i) / 10) = 20 AND y < viehy(i) AND feld(viehx(i) / 10, (viehy(i) - 30) / 10) = 0 THEN viehy(i) = viehy(i) - 30: viehr(i) = 2: black viehx(i), viehy(i) + 30, 4: monster viehx(i), viehy(i), 3, viehr(i): feld(viehx(i) / 10, viehy(i) / 10) = 20
IF x = viehx(i) AND y = viehy(i) AND feld(viehx(i) / 10, viehy(i) / 10) = 20 THEN
a = x
B = y
black a, B, 4
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

CASE 27
IF x = 570 AND y = 390 AND feld(57, 39) <> 20 THEN schlussel(1) = 1
IF x = 90 AND y = 270 THEN schlussel(2) = 1

CASE 28
IF x = 540 AND y = 390 THEN schlussel(1) = 1
IF x = 240 AND y = 240 THEN schlussel(2) = 1

CASE 30
IF x = 270 AND y = 240 THEN schlussel(1) = 1
IF x = 60 AND y = 240 THEN schlussel(2) = 1
IF x = 570 AND y = 60 THEN schlussel(3) = 1
IF x = 30 AND y = 30 THEN schlussel(4) = 1

CASE 32
IF x = 30 AND y = 180 THEN schlussel(1) = 1
IF x = 30 AND y = 240 THEN schlussel(2) = 1

CASE 33
IF x = 30 AND y = 390 THEN schlussel(1) = 1
IF x = 60 AND y = 390 THEN schlussel(2) = 1
IF x = 120 AND y = 390 THEN schlussel(3) = 1
IF x = 510 AND y = 30 THEN schlussel(4) = 1
IF x = 570 AND y = 30 THEN schlussel(5) = 1
IF x = 570 AND y = 210 THEN schlussel(6) = 1
IF x = 510 AND y = 390 THEN schlussel(7) = 1
IF x = 570 AND y = 390 THEN schlussel(8) = 1
IF x = 360 AND y = 360 THEN schlussel(9) = 1

CASE 34
IF x = 360 AND y = 30 THEN schlussel(1) = 1
IF x = 300 AND y = 360 THEN schlussel(4) = 1
IF x = 60 AND y = 30 THEN schlussel(3) = 1
IF x = 570 AND y = 330 THEN schlussel(2) = 1

CASE 35
IF x = 120 AND y = 120 THEN schlussel(1) = 1
IF x = 120 AND y = 270 THEN schlussel(2) = 1
IF x = 270 AND y = 120 THEN schlussel(3) = 1
IF x = 270 AND y = 270 THEN schlussel(4) = 1
IF x = 420 AND y = 120 THEN schlussel(5) = 1
IF x = 420 AND y = 270 THEN schlussel(6) = 1
IF x = 570 AND y = 120 THEN schlussel(8) = 1
IF x = 570 AND y = 270 THEN schlussel(7) = 1

CASE 37
IF schlussel(1) = 1 AND x = 150 AND y = 30 THEN schlussel(4) = 1: GOTO managedl37sch4
IF x = 150 AND y = 30 THEN schlussel(1) = 1
managedl37sch4:
IF x = 30 AND y = 390 THEN schlussel(2) = 1
IF x = 90 AND y = 390 THEN schlussel(3) = 1

CASE ELSE
END SELECT
EXIT SUB

wassertod2:
BEEP
RETURN

trnk:
SELECT CASE level
END SELECT
END SUB

SUB tisch (x, y, z)

feld(x / 10, y / 10) = 26

IF z = 1 THEN GOSUB Tisch1
IF z = 2 THEN GOSUB Tisch2

Tisch1:
feld((x + 30) / 10, y / 10) = 26
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
feld(x / 10, (y + 30) / 10) = 26
LINE (x + 0, y + 0)-(x + 30, y + 60), 6, BF
LINE (x + 0, y + 6)-(x + 30, y + 54), 0, B
LINE (x + 6, y + 6)-(x + 6, y + 54), 0
LINE (x + 12, y + 6)-(x + 12, y + 54), 0
LINE (x + 18, y + 6)-(x + 18, y + 54), 0
LINE (x + 24, y + 6)-(x + 24, y + 54), 0

3 END SUB

SUB tor (x, y, f)

feld(x / 10, y / 10) = 21
COLOR f
LINE (x + 0, y + 0)-(x + 30, y + 30), , B
PAINT (x + 12, y + 12)
CIRCLE (x + 15, y + 12), 4, 0
PAINT (x + 15, y + 12), 0
LINE (x + 13, y + 20)-(x + 17, y + 15), 0, B
PAINT (x + 14, y + 17), 0

END SUB

SUB tresor (x, y, z)

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

SUB wasser (x, y)

feld(x / 10, y / 10) = 27
COLOR 1
LINE (x + 0, y + 0)-(x + 30, y + 30), 1, BF

END SUB

