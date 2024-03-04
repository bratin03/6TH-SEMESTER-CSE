import { useState } from "react";
import { TailSpin } from "react-loader-spinner";

const Button = ({ className, onClick, type, text, loading }) => {
  return (
    <button
      className={
        loading
          ? `${className} flex flex-row gap-4 justify-center bg-slate-400 text-white cursor-not-allowed`
          : className
      }
      onClick={onClick}
      type={type}
      disabled={loading}
    >
      {loading && (
        <TailSpin height={20} width={20} color="white" ariaLabel="loading" />
      )}
      {text}
    </button>
  );
};

export default Button;
