vim.lsp.config("clangd", {
    root_dir = vim.fn.fnamemodify(debug.getinfo(1, "S").source:sub(2), ":p:h"),
})
