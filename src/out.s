
.data

a: .word 0
_ch0: .asciiz "c = "

.text

    addiu $29, $29, -4
    lui   $8, 0x1001
    lw    $8, 0($8)
    nor   $9, $0, $8
    sw    $9, 0($29)
    lui   $4, 0x1001
    ori   $4, $4, 0x4
    ori   $2, $0, 0x4
    syscall
    lw    $4, 0($29)
    ori   $2, $0, 0x1
    syscall
    ori   $2, $0, 0xa
    syscall
