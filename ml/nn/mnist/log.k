\d .LOG

tnames:`error`warn`info`trace
verbosity: 3

log:{[fac;sev;msg]
}

f:{[fac]
    d:!+/((`t;{[f;m] log[f;3;m]}[fac])
        (`i;{[f;m] log[f;2;m]}[fac])
        (`w;{[f;m] log[f;1;m]}[fac])
        (`e;{[f;m] log[f;0;m]}[fac]))
    :d
}

\d .
