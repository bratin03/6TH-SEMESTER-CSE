import { Drama } from "lucide-react";
import { Link } from "react-router-dom";

const Logo = () => {
  return (
    <Link to="/" className="flex gap-2 items-center">
      <Drama className="w-8 h-8" />
      <span className="font-[bonny] text-3xl font-black">odyssey</span>
    </Link>
  );
};

export default Logo;
