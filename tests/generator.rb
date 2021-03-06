#!/usr/bin/ruby

class Generator
    
    def initialize
    end

    def program
        "#{function_}#{functions_};"
    end
    
private
    def functions_
        case rand(2)
            when 0 then "#{function_}#{functions_}"
            else ""
        end
    end

    def function_
        "function #{identifier_} ( #{formal_parameters_} ) \n" +
          "#{locals_}" +
          "#{statement_seq_ 1}" +
        "end\n\n"
    end

    def identifier_
        # prvni znak musi byt takovy, aby to nahodou nevyslo na klicove slovo
        i = "z"
        begin
            r = rand(37)
            if r > 26
                i += (r-26).to_s
            elsif r == 26
                i += "_"
            else
                i += ('a'.ord + r).chr
            end
        end while rand(5) != 0 
        i
    end

    def formal_parameters_
        case rand(3)
            when 0 then ""
            else "#{identifier_}#{formal_parameters_z}"
        end
    end

    def formal_parameters_z
        case rand(2)
            when 0 then ""
            when 1 then ", #{identifier_}#{formal_parameters_z}"
        end
    end

    def locals_
        case rand(3)
            when 0 then "  local #{identifier_};\n#{locals_}"
            when 1 then "  local #{identifier_} = #{literal_};\n#{locals_}"
            else "\n"
        end
    end

    def literal_
        case rand(5)
            when 0 then "#{rand(32768)}"
            when 1 then "\"#{identifier_}\""
            when 2 then "nil"
            when 3 then "true"
            else "false"
        end
    end

    def statement_seq_ n
        return "" if n > 3
        case rand(5)
            when 0 then ""
            else "#{statement_ n};\n#{statement_seq_ n}"
        end
    end

    def statement_ n
        case rand(10)
            when 0..2 then
                "  "*n + "write(#{expression_list_})"
            when 3 then
                "  "*n + "if #{expression_} then\n" +
                "#{statement_seq_ n+1}" +
                "  "*n + "else\n" +
                "#{statement_seq_ n+1}" +
                "  "*n + "end"
            when 4 then
                "  "*n + "while #{expression_} do\n" +
                "#{statement_seq_ n+1}\n" +
                "  "*n + "end"
            when 5..7 then
                "  "*n + "return #{expression_}"
            when 8..9 then
                "  "*n + "#{identifier_} = read(\"#{identifier_}\")"
        end
    end

    def expression_
        "1"
    end

    def expression_list_
        "1, 3, 4"
    end
end

g  = Generator.new
puts g.program
