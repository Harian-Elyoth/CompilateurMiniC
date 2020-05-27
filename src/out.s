
.data


.text

    addiu $29, $29, -4
    ori   $9, $0, 0x2
    subu  $9, $0, $9
    sw    $9, 0($29)
    ori   $2, $0, 0xa
    syscall
