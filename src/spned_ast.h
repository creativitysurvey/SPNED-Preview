#pragma once
#include <string>
#include <vector>
#include <memory>

// 基础语法树节点
struct ASTNode {
    virtual ~ASTNode() = default;
};

// ==========================================
// 表达式节点 (Expressions)
// ==========================================
struct ExprNode : ASTNode {};

struct IntExprNode : ExprNode {
    int value;
    IntExprNode(int v) : value(v) {}
};

struct VarExprNode : ExprNode {
    std::string name;
    VarExprNode(const std::string& n) : name(n) {}
};

struct BinaryExprNode : ExprNode {
    std::string op; 
    std::unique_ptr<ExprNode> left, right;
    BinaryExprNode(std::string o, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

struct BufferAccessNode : ExprNode {
    std::string buffer_name; 
    std::unique_ptr<ExprNode> index;
    BufferAccessNode(const std::string& b, std::unique_ptr<ExprNode> idx)
        : buffer_name(b), index(std::move(idx)) {}
};

// ==========================================
// 语句节点 (Statements)
// ==========================================
struct StmtNode : ASTNode {};

struct AssignNode : StmtNode {
    std::unique_ptr<ExprNode> target; 
    std::unique_ptr<ExprNode> value;
    AssignNode(std::unique_ptr<ExprNode> t, std::unique_ptr<ExprNode> v)
        : target(std::move(t)), value(std::move(v)) {}
};

struct ForLoopNode : StmtNode {
    std::string iterator_name;
    std::unique_ptr<ExprNode> start_expr;
    std::unique_ptr<ExprNode> end_expr;
    std::vector<std::unique_ptr<StmtNode>> body;

    ForLoopNode(std::string iter, std::unique_ptr<ExprNode> s, std::unique_ptr<ExprNode> e)
        : iterator_name(iter), start_expr(std::move(s)), end_expr(std::move(e)) {}
};
