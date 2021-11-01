#include <yed/plugin.h>
#include <yed/highlight.h>

#define ARRAY_LOOP(a) for (__typeof((a)[0]) *it = (a); it < (a) + (sizeof(a) / sizeof((a)[0])); ++it)

highlight_info hinfo;

void
unload(yed_plugin* self);
void
syntax_rust_line_handler(yed_event* event);
void
syntax_rust_frame_handler(yed_event* event);
void
syntax_rust_buff_mod_pre_handler(yed_event* event);
void
syntax_rust_buff_mod_post_handler(yed_event* event);

int
yed_plugin_boot(yed_plugin* self)
{
    yed_event_handler frame, line, buff_mod_pre, buff_mod_post;

    char* kwds[] =
    {
        "lui",       "auipc",      "addi",      "slti",     "sltiu",
        "xori",      "ori",        "andi",      "slli",     "srli",
        "srai",      "add",        "sub",       "sll",      "slt",
        "sltu",      "xor",        "srl",       "sra",      "or",
        "and",       "fence",      "fence.i",   "csrrw",    "csrrs",
        "csrrc",     "csrrwi",     "csrrsi",    "csrrci",   "ecall",
        "ebreak",    "sfence.vma", "lb",        "lh",       "lw",
        "lbu",       "lhu",        "sb",        "sh",       "sw",
        "addiw",     "slliw",      "srliw",     "sraiw",    "addw",
        "subw",      "sllw",       "srlw",      "sraw",     "lwu",
        "ld",        "sd",         "fmadd.s",   "fmsub.s",  "fnmsub.s",
        "fnmadd.s",  "fadd.s",     "fsub.s",    "fmul.s",   "fdiv.s",
        "fsqrt.s",   "fsgnj.s",    "fsgnjn.s",  "fsgnjx.s", "fmin.s",
        "fmax.s",    "fcvt.w.s",   "fcvt.wu.s", "fmv.x.w",  "feq.s",
        "flt.s",     "fle.s",      "fclass.s",  "fcvt.s.w", "fcvt.s.wu",
        "fmv.w.x",   "fmadd.d",    "fmsub.d",   "fnmsub.d", "fnmadd.d",
        "fadd.d",    "fsub.d",     "fmul.d",    "fdiv.d",   "fsqrt.d",
        "fsgnj.d",   "fsgnjn.d",   "fsgnjx.d",  "fmin.d",   "fmax.d",
        "fcvt.s.d",  "fcvt.d.s",   "feq.d",     "flt.d",    "fle.d",
        "fclass.d",  "fcvt.w.d",   "fcvt.wu.d", "fcvt.d.w", "fcvt.d.wu",
        "flw",       "fsw",        "fld",       "fsd",      "fcvt.l.s",
        "fcvt.lu.s", "fcvt.s.l",   "fcvt.s.lu",

    };
    char* special_kwds[] =
    {
        "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",  "x8",  "x9",
        "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19",
        "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29",
        "x30", "x31", "f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",
        "f8",  "f9",  "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17",
        "f18", "f19", "f20", "f21", "f22", "f23", "f24", "f25", "f26", "f27",
        "f28", "f29", "f30", "f31",
    };
    char* control_flow[] =
    {
        "uret", "sret", "mret", "wfi", "jal",  "jalr",
        "beq",  "bne",  "blt",  "bge", "bltu", "bgeu",
    };
    char* typenames[] =
    {
        "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",  "x8",  "x9",
        "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19",
        "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29",
        "x30", "x31", "f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",
        "f8",  "f9",  "f10", "f11", "f12", "f13", "f14", "f15", "f16", "f17",
        "f18", "f19", "f20", "f21", "f22", "f23", "f24", "f25", "f26", "f27",
        "f28", "f29", "f30", "f31",
    };

    YED_PLUG_VERSION_CHECK();

    yed_plugin_set_unload_fn(self, unload);

    frame.kind = EVENT_FRAME_PRE_BUFF_DRAW;
    frame.fn = syntax_rust_frame_handler;
    line.kind = EVENT_LINE_PRE_DRAW;
    line.fn = syntax_rust_line_handler;
    buff_mod_pre.kind = EVENT_BUFFER_PRE_MOD;
    buff_mod_pre.fn = syntax_rust_buff_mod_pre_handler;
    buff_mod_post.kind = EVENT_BUFFER_POST_MOD;
    buff_mod_post.fn = syntax_rust_buff_mod_post_handler;

    yed_plugin_add_event_handler(self, frame);
    yed_plugin_add_event_handler(self, line);
    yed_plugin_add_event_handler(self, buff_mod_pre);
    yed_plugin_add_event_handler(self, buff_mod_post);

    highlight_info_make(&hinfo);

    ARRAY_LOOP(kwds)
    highlight_add_kwd(&hinfo, *it, HL_KEY);
    ARRAY_LOOP(special_kwds)
    highlight_add_kwd(&hinfo, *it, HL_PP);
    ARRAY_LOOP(control_flow)
    highlight_add_kwd(&hinfo, *it, HL_CF);
    ARRAY_LOOP(typenames)
    highlight_add_kwd(&hinfo, *it, HL_TYPE);
    highlight_add_kwd(&hinfo, "zero", HL_CON);
    highlight_within(&hinfo, "(", ")", 0, -1, HL_CF);
    highlight_numbers(&hinfo);
    highlight_within(&hinfo, "\"", "\"", '\\', -1, HL_STR);
    highlight_within(&hinfo, "'", "'", '\\', 1, HL_CHAR);
    highlight_to_eol_from(&hinfo, "#", HL_COMMENT);

    ys->redraw = 1;

    return 0;
}

void
unload(yed_plugin* self)
{
    highlight_info_free(&hinfo);
    ys->redraw = 1;
}

void
syntax_rust_frame_handler(yed_event* event)
{
    yed_frame* frame;

    frame = event->frame;

    if (!frame || !frame->buffer || frame->buffer->kind != BUFF_KIND_FILE ||
            frame->buffer->ft != yed_get_ft("RV64ASM"))
    {
        return;
    }

    highlight_frame_pre_draw_update(&hinfo, event);
}

void
syntax_rust_line_handler(yed_event* event)
{
    yed_frame* frame;

    frame = event->frame;

    if (!frame || !frame->buffer || frame->buffer->kind != BUFF_KIND_FILE ||
            frame->buffer->ft != yed_get_ft("RV64ASM"))
    {
        return;
    }

    highlight_line(&hinfo, event);
}

void
syntax_rust_buff_mod_pre_handler(yed_event* event)
{
    if (event->buffer == NULL || event->buffer->kind != BUFF_KIND_FILE ||
            event->buffer->ft != yed_get_ft("RV64ASM"))
    {
        return;
    }

    highlight_buffer_pre_mod_update(&hinfo, event);
}

void
syntax_rust_buff_mod_post_handler(yed_event* event)
{
    if (event->buffer == NULL || event->buffer->kind != BUFF_KIND_FILE ||
            event->buffer->ft != yed_get_ft("RV64ASM"))
    {
        return;
    }

    highlight_buffer_post_mod_update(&hinfo, event);
}