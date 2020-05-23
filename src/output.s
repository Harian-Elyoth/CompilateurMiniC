
.text

    addiu $29, $29, -12
    ori   $8, $0, 0x5
    sw    $8, 0($29)
    ori   $8, $0, 0x6
    sw    $8, 4($29)
    addu  $9, $9, $10
    sltu  $9, $9, $10
    ori   $10, $0, 0x0
    bne   $9, $10, _L1
    j     _L2
_L1:
    ori   $9, $0, 0x1
_L2:
