#include <iostream>
#include <memory>

struct Request {
    enum class Type {
        Email,
        Password,
        Age
    };
    explicit Request(Type t) : type(t) {}
    virtual ~Request() = default;
    Type type;
};

struct Email : Request {
    Email() : Request(Type::Email) {};
    std::string email;
};

struct Password : Request {
    Password() : Request(Type::Password) {};
    std::string password;
};

struct Age : Request {
    Age() : Request(Type::Age) {};
    int age;
};

class Handler {
public:
    virtual ~Handler() = default;
    virtual std::shared_ptr<Handler> setNext(std::shared_ptr<Handler>) = 0;
    virtual bool process(const Request&) = 0;
};

class ValidationHandler : public Handler {
public:
    std::shared_ptr<Handler> setNext(std::shared_ptr<Handler> handler) override {
        next = handler;
        return next;
    }
    bool process(const Request& request) override {
        return next ? next->process(request) : true; 
    }
protected:
    std::shared_ptr<Handler> next = nullptr;
};

class EmailHandler : public ValidationHandler {
    bool process(const Request& request) override {
        const Email& emailReq = static_cast<const Email&>(request);
        if (emailReq.type != Request::Type::Email) {
            std::cout << "  EmailValidator: passed requests doesn't have \'email\' field" << '\n';
        } else {
            if (emailReq.email.find('@') != std::string::npos)
            std::cout << "  EmailValidator: email is valid" << '\n';
            else {
                std::cout << "  EmailValidator: email is invalid. Should contain \'at\' symbol (@)" << '\n';
                return false; 
            }
        }

        return next ? next->process(request) : true;
    }
};

class PasswordHandler : public ValidationHandler {
    bool process(const Request& request) override {
        const Password& passReq = static_cast<const Password&>(request);
        if (passReq.type != Request::Type::Password) {
            std::cout << "  PasswordValidator: passed requests doesn't have \'password\' field" << '\n';
        } else {
            if (passReq.password.length() >= 6)
                std::cout << "  PasswordValidator: Password is valid" << '\n';
            else {
                std::cout << "  PasswordValidator: is invalid. Length should be greater than 6" << '\n';
                return false;
            }
        }
        
        return next ? next->process(request) : true;
    }
};

class AgeHandler : public ValidationHandler {
    bool process(const Request& request) override {
        const Age& ageReq = static_cast<const Age&>(request);
        if (ageReq.type != Request::Type::Age) {
            std::cout << "  AgeValidator: passed requests doesn't have \'age\' field" << '\n';
        } else {
            if (ageReq.age > 0 && ageReq.age < 120)
                std::cout << "  AgeValidator: age is valid" << '\n';
            else {
                std::cout << "  AgeValidator: Invalid age. Ranges [1; 119]" << '\n';
                return false;
            }
        }

        return next ? next->process(request) : true;
    }
};


int main() {
    std::shared_ptr<Handler> emailHandler = std::make_shared<EmailHandler>();
    std::shared_ptr<Handler> passHandler = std::make_shared<PasswordHandler>();
    std::shared_ptr<Handler> ageHandler = std::make_shared<AgeHandler>();

    emailHandler->setNext(passHandler)->setNext(ageHandler);

    Email emailReq{};
    emailReq.email = "example@example.com";
    Password passReq{};
    passReq.password = "123456";
    Age ageReq{};
    ageReq.age = 20;

    bool result;

    std::cout << "Chain: EmailValidator > PasswordValidator > AgeValidator\n\n";
    result = emailHandler->process(emailReq);

    std::cout << std::endl;
    std::cout << "Chain: PasswordValidator > AgeValidator\n\n";
    result = result & passHandler->process(passReq);

    std::cout << std::endl;
    std::cout << "Chain: AgeValidator\n\n";
    result = result & ageHandler->process(ageReq);

    std::cout << std::endl;
    if (result) {
        std::cout << "validation OK";
    } else {
        std::cout << "validation FAILED";
    }
    return 0;
}