segment .text
global main
extern printf
extern scanf
extern exit


main:
push rbp
mov rbp, rsp
sub rsp, 96
mov rax, 19
push rax
pop rbx
mov rax, rbp
sub rax, 8
sub rax, 8
mov [rax], rbx
mov rax, 56
push rax
pop rbx
mov rax, rbp
sub rax, 8
sub rax, 16
mov [rax], rbx
mov rax, rbp
sub rax, 40
sub rax, 8
mov rbx, 3
mov [rax], rbx
@for1:
mov rax, rbp
sub rax, 40
sub rax, 8
mov rcx, [rax]
mov rbx, 7
cmp rcx, rbx
jz end_@for1

push rax
push rbx
push rcx
push rdx

mov rdi, ip_int

push rax
push rbx
push rcx
push rdx

call printf

pop rdx
pop rcx
pop rbx
pop rax

mov rax, rbp
sub rax, 0
sub rax, 8
lea rsi, [rax]
mov rdi, f_ip_int
call scanf

pop rdx
pop rcx
pop rbx
pop rax

mov rax, rbp
sub rax, 0
sub rax, 8
mov rcx, [rax]
case_1_@switch1:
cmp rcx, 1
jnz case_2_@switch1
mov rax, rbp
sub rax, 0
sub rax, 8
mov rax, [rax]
push rax
mov rax, 2
push rax
pop rbx
pop rax
imul rax, rbx
push rax
mov rax, rbp
sub rax, 8
sub rax, 8
mov rax, [rax]
push rax
pop rbx
pop rax
sub rax, rbx
push rax
mov rax, rbp
sub rax, 40
sub rax, 8
mov rax, [rax]
push rax
pop rbx
pop rax
add rax, rbx
push rax
pop rbx
mov rax, rbp
sub rax, 8
sub rax, 8
mov [rax], rbx
mov rdi, op_int
mov rax, rbp
sub rax, 8
sub rax, 8
mov rsi, [rax]

push rax
push rbx
push rcx
push rdx

call printf

pop rdx
pop rcx
pop rbx
pop rax

jmp end_@switch1
case_2_@switch1:
cmp rcx, 2
jnz default_@switch1
mov rax, rbp
sub rax, 0
sub rax, 8
mov rax, [rax]
push rax
mov rax, 3
push rax
pop rbx
pop rax
imul rax, rbx
push rax
mov rax, rbp
sub rax, 16
sub rax, 8
mov rax, [rax]
push rax