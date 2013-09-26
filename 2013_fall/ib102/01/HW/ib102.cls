% ib102.sty
% style for homeworks for IB102 Automata, Grammars, and Complexity
% (c) 2013 Vladimír Štill, CC BY 3.0 (https://creativecommons.org/licenses/by/3.0/)

\ProvidesClass{ib102}[2013/06/29 v 0.1 IB102]
\NeedsTeXFormat{LaTeX2e}


\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}

\ProcessOptions \relax
\LoadClass{article}

\RequirePackage[utf8]{inputenc}
\RequirePackage[a4paper, left=2.5cm, right=2.5cm, top=2.5cm, bottom=2.5cm]{geometry}
\RequirePackage[T1]{fontenc}
\RequirePackage{fancyhdr}
\RequirePackage{ifthen}

\RequirePackage{pgf}
\RequirePackage{tikz}
\RequirePackage{amssymb}
\RequirePackage{amsmath}
\RequirePackage{amsthm}   %důkazy
\usetikzlibrary{calc,arrows,automata}

\newcommand{\eps}{\varepsilon}
\newcommand{\coL}{co\mbox{$-$}L}
\newcommand{\move}{\rightarrow}
\newcommand{\set}[1]{\ensuremath{\{ #1 \}}}

%% BASIC COMMANDS

\addtolength{\headheight}{2.5cm}
\addtolength{\textheight}{-2.5cm}
\addtolength{\headsep}{-0.4cm}

\def\@student{}
\newcommand{\setstudent}[1]{\def\@student{#1}}

\def\@group{}
\newcommand{\setgroup}[1]{\def\@group{#1}}

\def\@uco{\hspace{3cm}$ $}
\newcommand{\setuco}[1]{\def\@uco{#1}}

\def\@assignment{0}
\newcommand{\setassignment}[1]{\def\@assignment{#1}}

\def\@exercise{0}
\newcommand{\setexercise}[1]{\def\@exercise{#1}}

\def\@points{0}
\newcommand{\setpoints}[1]{\def\@points{#1}}

\def\@dueday{1}
\def\@duemonth{9}
\def\@dueyear{2013}
\newcommand{\setduedate}[3]{\def\@dueday{#1}\def\@duemonth{#2}\def\@dueyear{#3}}

\newcommand{\body}[1]{#1 \ifthenelse{#1 = 1}{bod}{\ifthenelse{#1 = 0 \or #1 > 4}{bodů}{body}}}

%% HEADER
\pagestyle{fancyplain}
\fancyhf{}

\lhead{\large
    \textbf{IB102 -- úkol \@assignment, příklad \@exercise
        \ifx\@solution\@empty
        {} -- řešení
        \fi
        \hfill Odevzdání: \@dueday.\,\@duemonth.\,\@dueyear}
    \vspace{2mm}
    \hrule
    \vspace{2mm}
    Vypracoval(a): \@student\hfill{}UČO: \@uco \\
    Skupina: \@group
    \vspace{1mm}
}

%% DOCUMENT ENVIRONMENTS

\newenvironment{zadani}{%
    \noindent
    \textbf{\@exercise.\, [\body{\@points}]}
}{\bigskip\bigskip\noindent}

\newenvironment{reseni}{\global\let\@solution\@empty}{}

\cfoot{\thepage}


