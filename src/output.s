
.data

a: .word 5

.text

    addiu $29, $29, -8
    lui   $8, 0x1001
    lw    $8, 0($8)
    sw    $8, 0($29)
    ori   $8, $0, 0x4
    sw    $8, 4($29)
