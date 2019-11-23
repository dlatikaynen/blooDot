Attribute VB_Name = "Modul1"
Option Explicit

Sub PrepareSheet()
Attribute PrepareSheet.VB_ProcData.VB_Invoke_Func = " \n14"
'
' PrepareSheet Makro
'

'
    Dim I As Byte
    Cells.Select
    Selection.ColumnWidth = 1.89
    I = 0
    Do
        DrawConfiguration I, 2 + I * 4, 2
        If I = 255 Then Exit Do
        I = I + 1
    Loop
End Sub

Public Sub DrawConfiguration(neighborConfiguration As Byte, offsetRow As Integer, offsetColumn As Integer)
    Dim I As Byte
    Dim cellTL As Range
    Dim cellBR As Range
    Dim cellCenter As Range
    Dim centerRow As Integer
    Dim centerCol As Integer
    Dim cellNeighbors(1 To 8) As Range
    Set cellTL = Cells(offsetRow, offsetColumn)
    Set cellBR = Cells(offsetRow + 2, offsetColumn + 2)
    centerRow = offsetRow + 1
    centerCol = offsetColumn + 1
    Set cellCenter = Cells(centerRow, centerCol)
    Set cellNeighbors(1) = Cells(centerRow - 1, centerCol - 1)
    Set cellNeighbors(2) = Cells(centerRow - 1, centerCol)
    Set cellNeighbors(3) = Cells(centerRow - 1, centerCol + 1)
    Set cellNeighbors(4) = Cells(centerRow, centerCol + 1)
    Set cellNeighbors(5) = Cells(centerRow + 1, centerCol + 1)
    Set cellNeighbors(6) = Cells(centerRow + 1, centerCol)
    Set cellNeighbors(7) = Cells(centerRow + 1, centerCol - 1)
    Set cellNeighbors(8) = Cells(centerRow, centerCol - 1)
    Range(cellTL, cellBR).Select
    Selection.Borders(xlDiagonalDown).LineStyle = xlNone
    Selection.Borders(xlDiagonalUp).LineStyle = xlNone
    With Selection.Borders(xlEdgeLeft)
        .LineStyle = xlContinuous
        .ColorIndex = 0
        .TintAndShade = 0
        .Weight = xlThin
    End With
    With Selection.Borders(xlEdgeTop)
        .LineStyle = xlContinuous
        .ColorIndex = 0
        .TintAndShade = 0
        .Weight = xlThin
    End With
    With Selection.Borders(xlEdgeBottom)
        .LineStyle = xlContinuous
        .ColorIndex = 0
        .TintAndShade = 0
        .Weight = xlThin
    End With
    With Selection.Borders(xlEdgeRight)
        .LineStyle = xlContinuous
        .ColorIndex = 0
        .TintAndShade = 0
        .Weight = xlThin
    End With
    With Selection.Borders(xlInsideVertical)
        .LineStyle = xlContinuous
        .ColorIndex = 0
        .TintAndShade = 0
        .Weight = xlThin
    End With
    With Selection.Borders(xlInsideHorizontal)
        .LineStyle = xlContinuous
        .ColorIndex = 0
        .TintAndShade = 0
        .Weight = xlThin
    End With
    cellCenter.Select
    With Selection.Interior
        .Pattern = xlSolid
        .PatternColorIndex = xlAutomatic
        .ThemeColor = xlThemeColorAccent1
        .TintAndShade = -0.249977111117893
        .PatternTintAndShade = 0
    End With
    
    For I = 1 To 8
        If (neighborConfiguration And (2 ^ (I - 1))) = (2 ^ (I - 1)) Then
            cellNeighbors(I).Select
            With Selection.Interior
                .Pattern = xlSolid
                .PatternColorIndex = xlAutomatic
                .ThemeColor = xlThemeColorAccent2
                .TintAndShade = -0.249977111117893
                .PatternTintAndShade = 0
            End With
        End If
    Next I
End Sub
