
.data

a: .word 0
b: .word 2
.asciiz "la variable c vaut"
.asciiz "et doit valoir .."

.text

main:
    addiu $29, $29, -4
    lui   $8, 0x1001
    lw    $8, 0($8)
    lui   $9, 0x1001
    lw    $9, 4($9)
    srlv  $8, $8, $9
    sw    $8, 0($29)
    lui   $4, 0x1001
    ori   $4, $4, 0x8
    ori   $2, $0, 0x4
    syscall
    lw    $4, 0($29)
    ori   $2, $0, 0x1
    syscall
    lui   $4, 0x1001
    ori   $4, $4, 0x1b
    ori   $2, $0, 0x4
    syscall
    addiu $29, $29, 4
    ori   $2, $0, 0xa
    syscall
