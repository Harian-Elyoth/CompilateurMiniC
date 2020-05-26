
.data

_ch0: .asciiz "les variables c d e f valent "
_ch1: .asciiz " et doivent valoir 1 0 0 1"

.text

    addiu $29, $29, -28
    ori   $8, $0, 0x4
    sw    $8, 0($29)
    ori   $8, $0, 0x5
    sw    $8, 4($29)
    lw    $8, 0($29)
    lw    $10, 4($29)
    sltu  $9, $8, $10
    bne   $9, $0, _L1
    ori   $9, $0, 0x0
    j     _L2
_L1:
    ori   $9, $0, 0x1
_L2:
    sw    $9, 8($29)
    lw    $8, 0($29)
    lw    $10, 4($29)
    sltu  $9, $10, $8
    bne   $9, $0, _L3
    ori   $9, $0, 0x0
    j     _L4
_L3:
    ori   $9, $0, 0x1
_L4:
    sw    $9, 12($29)
    lw    $8, 0($29)
    lw    $10, 4($29)
    sltu  $9, $8, $10
    bne   $9, $0, _L5
    ori   $9, $0, 0x1
    j     _L6
_L5:
    ori   $9, $0, 0x0
_L6:
    sw    $9, 16($29)
    lw    $8, 0($29)
    lw    $10, 4($29)
    sltu  $9, $10, $8
    bne   $9, $0, _L7
    ori   $9, $0, 0x1
    j     _L8
_L7:
    ori   $9, $0, 0x0
_L8:
    sw    $9, 20($29)
    lui   $4, 0x1001
    ori   $4, $4, 0x0
    ori   $2, $0, 0x4
    syscall
    lw    $4, 8($29)
    ori   $2, $0, 0x1
    syscall
    lw    $4, 12($29)
    ori   $2, $0, 0x1
    syscall
    lw    $4, 16($29)
    ori   $2, $0, 0x1
    syscall
    lw    $4, 20($29)
    ori   $2, $0, 0x1
    syscall
    lui   $4, 0x1001
    ori   $4, $4, 0x1f
    ori   $2, $0, 0x4
    syscall
    ori   $2, $0, 0xa
    syscall
