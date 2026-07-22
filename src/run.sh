#!/bin/bash

# Rodar a partir da pasta src/
EXEC="./ted"
BASE="../t2"
SAIDA="../saida"

QRYS=(
    "00-perc-smpl"
    "01-perc-smpl-volta"
    "01-regs-quadr"
    "02-exp-apos-reg"
    "02-perc-estr3"
    "03-perc-apos-reg"
    "03-perc-smpl-intercalado"
    "04-perc-apos-exp"
    "05-perc-apos-reg-exp"
    "06-perc-zigzag"
    "07-perc-apos-exp"
    "08-perc-apos-reg-exp"
)

CONJUNTOS=("c1" "c2" "c3")

for C in "${CONJUNTOS[@]}"; do
    echo "=============================="
    echo " Conjunto: $C"
    echo "=============================="

    GEO="$BASE/$C.geo"
    VIA="$BASE/$C-v.via"
    OUT="$SAIDA/$C"

    mkdir -p "$OUT"

    for QRY in "${QRYS[@]}"; do
        QRY_PATH="$BASE/$C/$QRY.qry"

        if [ ! -f "$QRY_PATH" ]; then
            echo "  [SKIP] $QRY.qry não encontrado"
            continue
        fi

        echo -n "  Rodando $QRY... "

        $EXEC -e "$BASE" -f "$C.geo" -o "$OUT" -v "$C-v.via" -q "$C/$QRY.qry" 2>/dev/null

        if [ $? -eq 0 ]; then
            echo "[OK]"
        else
            echo "FALHOU (exit code $?)"
        fi
    done

    echo ""
done

echo "Testes concluídos. Saídas em $SAIDA/"